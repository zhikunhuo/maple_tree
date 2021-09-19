/*
 * maple_tree/maple_tree.cpp
 *
 * maple tree library 
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 1.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 *
 */
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "maple_tree.h"
#include "maple_tree_state.h"
#include "maple_tree.hpp"
#include "maple_subtree_state.hpp"
#include "maple_tree_big_node.hpp"

mapTree::mapTree()
{
     _mt  = mtMalloc(); 
     _mas = new mapTreeState(_mt, 0, ULONG_MAX);
}

bool mapTree::nodeIsRoot(maple_enode node){
    return (mtGetNode(mtGetNode(node)->parent) == (maple_node_t*)_mt)?true:false;
}
int mapTree::rootExpand(void *entry)
{
    maple_enode oldRoot = _mt->ma_root;
    maple_type_t type   = maple_node_leaf_64;
    maple_node_t *node;
    void ** slots;
    unsigned long * pivots;
    int slot = 0;

    if (!_mas->masPrevAllocNode(1)) {
        return -1;
    }

    node   = (maple_node_t *)_mas->masPopNode();
    pivots = mtNodePivots(node, type);
    slots  = mtNodeSlots(node,type);
    node->parent  = mtSetParentRootNode(_mt);//set parent
    maple_enode enode = mtSetNode(node,type);
    //enode = mtSetParentRoot(enode);
    
    if (oldRoot) {
        slots[slot++]= (void *)oldRoot;
    }
 
    if (!_mas->masGetIndex() && slot) {
        slot--;
    } else if (_mas->_index > 1) {
        pivots[slot++] = _mas->_index - 1;
    }

    slots[slot]     = entry;
    _mas->masSetOffset(slot);
    pivots[slot++]  = _mas->_last;
    _mas->_depth    = 1;
    mtSetHeight(_mt, _mas->_depth);
    _mt->ma_root = enode;    
    _mas->masSetNode(enode);

    return slot;
}

bool mapTree::append(void *entry, unsigned long min, unsigned char end,
                        void *content)
{
    maple_type_t mt = mtGetNodetype(_mas->_node);
    void **slots    = mtNodeSlots(_mas->masGetNode(), mt);
    unsigned long *pivots   = mtNodePivots(_mas->masGetNode(), mt);
    unsigned char max_slots = mtGetSlotsCount(mt);
    unsigned char new_end;

    /*
        * slot store would happen if the last entry wasn't being split, so add
        * one.
      */
    new_end = end + 1;
    if (min < _mas->_index) {
        new_end++;
    }
    if (new_end >= max_slots) {
        return false;
    }
    if (new_end < max_slots - 1) {
        pivots[new_end] = pivots[end];
    }
    slots[new_end--] = content;    
    if (new_end < max_slots - 1) {
        pivots[new_end] = _mas->_last;
    }
    slots[new_end--] = entry;

    if (min < _mas->_index) {
        pivots[new_end] = _mas->_index - 1;
        _mas->_offset++;
    }

    //mas_update_gap(mas);
    return true;
}

bool mapTree::tryStoreNodeStore( unsigned long min, unsigned long max,
                    unsigned char end, void *entry,void *content,
                    maple_type_t mt, void **slots,
                    unsigned long *pivots)
{
    void  **dst_slots;
    unsigned long *dst_pivots;
    unsigned char dst_offset, new_end = end;
    unsigned char offset, offset_end;
    maple_node_t reuse, *newnode;
    unsigned char copy_size = 0;

    offset = offset_end = _mas->_offset;
    if (_mas->_last == max) {
        /* don't copy this offset */
        offset_end++;
    } else if (_mas->_last < max) {
        /* new range ends in this range */
        if (max == ULONG_MAX) {
            //mas_bulk_rebalance(mas, end, mt);
        }

        new_end++;
        offset_end = offset;
     } else if (_mas->_last == _mas->_max) {
        /* runs right to the end of the node */
        new_end = offset;
        /* no data beyond this range */
        offset_end = end + 1;
    } else {
        unsigned long piv = 0;

        new_end++;
        do {
            offset_end++;
            new_end--;
            piv = _mas->masGetLogicalPivot(pivots, offset_end, mt);
        }while (piv <= _mas->_last);
    }

    /* new range starts within a range */
    if (min < _mas->_index) {
        new_end++;
    }

    /* Not enough room */
    if (new_end >= mtGetPivotsCount(mt)) {
        return false;
    }

    /* Not enough data. */
    if (!nodeIsRoot(_mas->_node) && (new_end <= mtGetMinSlotsCount(mt))) {
         //!(mas->mas_flags & MA_STATE_BULK))
        return false;
    }

    /* set up node. */
    memset(&reuse, 0, sizeof(struct maple_node));
    newnode = &reuse;

    newnode->parent =_mas->masGetNode()->parent;

    dst_pivots = mtNodePivots(newnode, mt);
    dst_slots  = mtNodeSlots(newnode, mt);
    /* Copy from start to insert point */
    memcpy(dst_pivots, pivots, sizeof(unsigned long) * (offset + 1));
    memcpy(dst_slots, slots, sizeof(void *) * (offset + 1));
    dst_offset = offset;

    /* Handle insert of new range starting after old range */
    if (min < _mas->_index) {
        dst_slots[dst_offset] =content;
        dst_pivots[dst_offset++] = _mas->_index - 1;
    }

    /* Store the new entry and range end. */
    unsigned char max_piv = mtGetPivotsCount(mt);
    if (dst_offset < max_piv){
        dst_pivots[dst_offset] = _mas->_last;
    }
    _mas->_offset = dst_offset;
    dst_slots[dst_offset++] = entry;

    /* this range wrote to the end of the node. */
    if (offset_end > end) {
        goto done;
    }

    /* Copy to the end of node if necessary. */
    copy_size = end - offset_end + 1;
    memcpy(dst_slots + dst_offset, slots + offset_end, sizeof(void *) * copy_size);
    if (dst_offset < max_piv) {
        if (copy_size > max_piv - dst_offset)
            copy_size = max_piv - dst_offset;
        memcpy(dst_pivots + dst_offset, pivots + offset_end,
           sizeof(unsigned long) * copy_size);
    }
   
done:
    if ((end == mtGetSlotsCount(mt) - 1) && (new_end < mtGetSlotsCount(mt) - 1)) {
        dst_pivots[new_end] = _mas->_max;
    }

    memcpy(_mas->masGetNode(), newnode, sizeof(maple_node_t));

    //mas_update_gap(mas);
    return true;
}


bool mapTree::insertSlot(void *entry,
                        unsigned long min, unsigned long max,
                        unsigned char end, void *content,
                        void **slots)
{
    maple_node_t *node    = _mas->masGetNode();
    maple_type_t mt       = mtGetNodetype(_mas->_node);
    unsigned long *pivots = mtNodePivots(_mas->masGetNode(), mt);
    unsigned long lmax; /* Logical max. */
    unsigned char offset = _mas->_offset;
    
    unsigned char max_slots = mtGetSlotsCount(mt);
    if (min == _mas->_index && max == _mas->_last) {
        /* exact fit. */
        slots[offset] = entry;
        goto done;
    }

    /* out of room. */
    if (offset + 1 >= max_slots) {
        return false;
    }

    /* going to split a single entry. */
    if (max > _mas->_last) {
        if ((offset == end) &&
            append(entry, min, end, content)) {
                return true;
          }

        goto try_node_store;
    }
#if 0
    lmax = mas_logical_pivot(mas, pivots, offset + 1, mt);
    /* going to overwrite too many slots. */
    if (lmax < mas->last)
        goto try_node_store;

    if (min == mas->index) {
        /* overwriting two or more ranges with one. */
        if (lmax <= mas->last)
            goto try_node_store;

        /* Overwriting a portion of offset + 1. */
        slots[offset] = entry;
        pivots[offset] = mas->last;
        goto done;
    } else if (min < mas->index) {
        /* split start */
        /* Doesn't end on the next range end. */
        if (lmax != mas->last)
            goto try_node_store;
        if (offset + 1 < mt_pivots[mt])
            pivots[offset + 1] = mas->last;
        slots[offset + 1] = entry;
        pivots[offset] = mas->index - 1;
        mas->offset++; /* Keep mas accurate. */
        goto done;
    }
#endif

    return false;

done:
    //mas_update_gap(mas);
    return true;
try_node_store:
    return tryStoreNodeStore(min, max, end, entry,content, mt, slots, pivots);
}

bool mapTree::insert(unsigned long first, unsigned long end, void *entry){
    unsigned long rMax = 0;
    unsigned long rMin = 0;
    void *content = NULL;
    maple_type_t  mt;
    maple_node_t *node = NULL;
    void  **slots;
    unsigned char endIndex;

    if ((first > end) || (!entry)
        ||(!first && !end)) {
        return false;
    }
    
    if ((_mas->masStart()) || (_mas->masIsNone()) ||  _mas->_node == MAS_ROOT) {
        _mas->masSetRange(first, end);
        if (rootExpand(entry) < 0) {
            return false;
        }

        return true;
    }

    _mas->masSetRange(first, end);
    if (!mapTreeWalk(&rMin, &rMax, entry)) {
        return NULL;
    }

    mt    = _mas->masGetNodeType();
    node  = _mas->masGetNode();
    slots = _mas->masNodeSlots();
    content = slots[_mas->_offset];

    if ((content || (_mas->_last > rMax))) {
        return false;
    }

    endIndex = _mas->masDataEnd();
    if (insertSlot(entry,rMin, rMax, endIndex, content,slots)) {
        return true;
    }

    mapTreeBigNode bigNode(_mas, entry,endIndex);
    bigNode.init();
    //bigNode.showBigNode();
    return bigNode.commitBignode(_mas,endIndex);
}

unsigned long mapTree::getMinPivots(unsigned long *pivots, unsigned char offset)
{
    if (!offset) {
        return _mas->_min;
    }

    return pivots[offset - 1] + 1;
}

/* find the [index last] , return rang_min<index<range_max,  or the node max range when index > the node max value*/
void mapTree::nodeWalk(maple_type_t type,
                        unsigned long *range_min, unsigned long *range_max)
{
    unsigned long *pivots =  mtNodePivots(_mas->masGetNode(), type);
    void ** slot          = mtNodeSlots(_mas->masGetNode(), type);
    unsigned char offset, count;
    unsigned long min, max, index;
    
    if (mtNodeIsDense(type)) {
        //TODO: need to check
        (*range_max) = (*range_min) = _mas->_index;
        if (mtDeadNode(_mas->_node)) {
            return ;
        }
        _mas->_offset = _mas->_index = _mas->_min;
        return ;
    }
    
    if (mtDeadNode(_mas->_node)) {        
        return;
    }

    offset = _mas->_offset;
    min    = getMinPivots(pivots, offset);
    count  = mtGetPivotsCount(type);
    index  = _mas->_index;
    max    = pivots[offset];

    if (offset == count) {
        goto max;
    }

    //find [index ,last], index <max, but can not confirm last < max
    if (index <= max) {
        goto done;
    }

    // first pivots is zero
    if (!max && offset) {
        goto max;
    }

    offset++;
    min = max + 1;
    while (offset < count) {
        max  = pivots[offset];
        if (index <= max) {
            goto done;
        }

        if (!max) {
            break;
        }
        min  = max + 1;
        offset++;
    }
 max:
    max = _mas->_max;
 done:
    *range_max = max;
    *range_min = min;
    _mas->_offset = offset;    
}

bool mapTree::mapTreeWalk(unsigned long *range_min,
                        unsigned long *range_max,
                        void *entry)
{
    maple_type_t type;

    while (true) {
        type = mtGetNodetype(_mas->_node);
        _mas->_depth++;
        nodeWalk(type, range_min, range_max);
        if (_mas->masIsSpanWsr(*range_max, type, entry)) {
            return false;
        }

        if (mtNodeIsLeaf(type)) {
            return true;
        }

        /* Traverse. */
        _mas->_max = *range_max;
        _mas->_min = *range_min;
        _mas->_node = mtNodeGetSlot(_mas->_node, type, _mas->_offset);
        _mas->_offset = 0;
    }
    return true;
}

void mapTree::showNode(maple_enode node,int height, unsigned long *number,bool flag)
{
    maple_type_t type;
    unsigned long nodeCount = 0;
    type = mtGetNodetype(node);
    if (mtNodeIsDense(type)) {
        if (flag) {
            printf("there is no node\n");
        }
        return ;
    }

    unsigned long *pivots = mtNodePivots(mtGetNode(node), type);
    unsigned char  count  = mtGetPivotsCount(type);
    void ** slot          = mtNodeSlots(mtGetNode(node), type);
    if (flag) {
        for(int i=0;i<height; i++) {
            printf("\t");
        }
  
        printf("--height:%d,node:%p\n",height,node);
    }

    height++;
    for(int loop=0;loop<count; loop++) {
        if (flag) {
            for(int i=0;i<height; i++) {
                printf("\t");
            }
        }
        maple_type_t tempType= mtGetNodetype(slot[loop]);
        if (flag) {
            printf("index:%d, pivot:%lu, slot:%p,type:%d\n",loop,pivots[loop],slot[loop],tempType);
        }
        if (slot[loop]){
            if (!mtNodeIsDense(tempType)) {
                showNode(slot[loop],height,&nodeCount,flag);
            } else {
                nodeCount++;
            }
        }
    } 
    *number += nodeCount;
}

unsigned long mapTree::showAllNodes(){
    unsigned long number = 0;
    if (!_mt->ma_root) {
        printf("The tree is empty\n");
    } else {
        showNode(_mt->ma_root,0,&number,true);
    }
    return number;
}

unsigned long mapTree::getAllNodeCount(){
    unsigned long number = 0;
    if (!_mt->ma_root) {
        return number;
    } else {
        showNode(_mt->ma_root,0,&number,false);
    }
    return number;
}

void * mapTree::find(unsigned long *index, unsigned long max){
    return NULL;
}
