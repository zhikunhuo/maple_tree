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
#include "maple_tree.h"
#include "maple_tree_state.h"
#include "maple_tree.hpp"

mapTree::mapTree()
{
     _mt  = mtMalloc(); 
     _mas = new mapTreeState(_mt, 0, ULONG_MAX);
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
    node->parent  = set_parent_root(_mt);//set parent
    maple_enode enode = mtSetNode(node,type);
    enode = mtSetRootFlag(enode);
    
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

    mt    = mtGetNodetype(_mas->_node);
    node  = _mas->masGetNode();
    slots = mtNodeSlots(node, mt);
    content = slots[_mas->_offset];

    if ((content || (_mas->_last > rMax))) {
        return false;
    }

    endIndex = _mas->masDataEnd();
    return true;
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
    unsigned long *pivots = mtNodePivots(_mas->masGetNode(), type);
    void ** slot           = mtNodeSlots(_mas->masGetNode(), type);
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

void mapTree::showNode(maple_enode node,int height, unsigned long *number)
{
    maple_type_t type;
    unsigned long nodeCount = 0;
    type = mtGetNodetype(node);
    if (mtNodeIsDense(type)) {
        printf("there is no node\n");
        return ;
    }

    unsigned long *pivots = mtNodePivots(mtGetNode(node), type);
    unsigned char  count  = mtGetPivotsCount(type);
    void ** slot          = mtNodeSlots(mtGetNode(node), type);
    for(int i=0;i<height; i++) {
          printf("\t");
    }

    printf("--height:%d,node:%p\n",height,node);
    height++;
    for(int loop=0;loop<count; loop++) {
        for(int i=0;i<height; i++) {
            printf("\t");
        }
        
        maple_type_t tempType= mtGetNodetype(slot[loop]);
        printf("index:%d, pivot:%lu, slot:%p,type:%d\n",loop,pivots[loop],slot[loop],tempType);
        if (slot[loop]){
            if (!mtNodeIsDense(tempType)) {
                showNode(slot[loop],height,&nodeCount);
            } else {
                nodeCount++;
            }
        }
    } 
    *number = nodeCount;
}

unsigned long mapTree::showAllNodes(){
    unsigned long number = 0;
    if (!_mt->ma_root) {
        printf("The tree is empty\n");
    } else {
        showNode(_mt->ma_root,0,&number);
    }
    return number;
}

void * mapTree::find(unsigned long *index, unsigned long max){
    return NULL;
}
