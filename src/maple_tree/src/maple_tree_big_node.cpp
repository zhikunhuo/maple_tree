/*
 * maple_tree/maple_tree_big_node.cpp
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
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree_state.h"
#include "maple_tree.h"
#include "maple_subtree_state.hpp"
#include "maple_tree_big_node.hpp"

mapTreeBigNode::mapTreeBigNode(mapTreeState * mas,void *entry,unsigned char end)
{
    _mas   = mas;
    _entry = entry;
    _end   = end;
}


void mapTreeBigNode::copyNodeToBigNode(mapTreeState * mas,
                                                unsigned char mas_start,
                                               unsigned char mas_end,
                                               unsigned char mab_start)
{
    maple_type_t mt;
    maple_node_t *node = mas->masGetNode();
    void  **slots;
    unsigned long *pivots, *gaps;
    int i = mas_start, j = mab_start;
    unsigned char piv_end;

    mt = mtGetNodetype(mas->_node);
    pivots = mtNodePivots(node, mt);
    if (!i) {
        _pivot[j] = pivots[i++];
        if (i > mas_end)
            goto complete;
        j++;
    }

    piv_end = min(mas_end, mtGetPivotsCount(mt));
    for (; i < piv_end; i++, j++) {
        _pivot[j] = pivots[i];
        if (!_pivot[j])
            break;

        if (mas->_max == _pivot[j])
            goto complete;
    }

    if (i <= mas_end)
        _pivot[j] = mas->masGetPivot(pivots, i, mt);

complete: 
    _b_end = ++j;
    j -= mab_start;
    slots = mtNodeSlots(node, mt);
    memcpy(_slot + mab_start, slots + mas_start, sizeof(void *) * j);
    if (!mtNodeIsLeaf(mt) &&  mas->masTreeIsAlloc()) {
        //gaps = ma_gaps(node, mt);
        //memcpy(b_node->gap + mab_start, gaps + mas_start,
        //       sizeof(unsigned long) * j);
    }
}

void mapTreeBigNode::copyBigNodeToNode(unsigned char mab_start, unsigned char mab_end)
{
    int i, j = 0;
    maple_type_t mt  = mtGetNodetype(_mas->_node);
    maple_node *node = _mas->masGetNode();
    void  **slots    = mtNodeSlots(node, mt);
    unsigned long *pivots = mtNodePivots(node, mt);
    unsigned long *gaps = NULL;

    if (mab_end - mab_start > mtGetPivotsCount(mt)) {
        mab_end--;
     }

    i = mab_start;
    pivots[j++] = _pivot[i++];
    do {
        pivots[j++] = _pivot[i++];
    } while (i <= mab_end && (_pivot[i]));

    memcpy(slots, _slot + mab_start, sizeof(void *) * (i - mab_start));

    _mas->_max = _pivot[i - 1]; 

    //process maple_arange_64 type gap
    #if 0
    if ((!mtNodeIsLeaf(mt) && _mas->masTreeIsAlloc())) {
        unsigned long max_gap = 0;
        unsigned char offset = 15;
        unsigned char end = j - 1;

        //set gap ,and find the max_gap and the offset which point the max gap
        gaps = ma_gaps(node, mt);
        do {
            gaps[--j] = b_node->gap[--i];
            if (gaps[j] > max_gap) {
                offset = j;
                max_gap = gaps[j];
            }
        } while (j);
        ma_set_meta(node, mt, offset, end);
    }
    #endif
}

unsigned char mapTreeBigNode::storeBignode(void *entry, unsigned char end)
{
    unsigned char slot    = _mas->_offset;
    void *contents;
    unsigned char b_end  = 0;
    /* Possible underflow of piv will wrap back to 0 before use. */
    unsigned long piv     = _mas->_min - 1;
    maple_node_t *node    = _mas->masGetNode();
    enum maple_type mt    = mtGetNodetype(_mas->_node);
    unsigned long *pivots = mtNodePivots(node, mt);
    void ** slots;

    if (slot) {
        /* Copy start data up to insert. */
        copyNodeToBigNode(_mas,0,(slot - 1),0);
        b_end = _b_end;
        piv   =_pivot[b_end - 1];
    }
    slots    = mtNodeSlots(node, mt);
    contents = slots[slot];    
    if (piv + 1 < _mas->_index) {
        /* Handle range starting after old range */
        _slot[b_end] = contents;
        if (!contents) {
            _gap[b_end] = _mas->_index - 1 - piv;
        }
        _pivot[b_end++] = _mas->_index - 1;
    }
    /* Store the new entry. */
    _mas->_offset = b_end;
    _slot[b_end] = entry;
    _pivot[b_end] = _mas->_last;

    /* Handle new range ending before old range ends */
    piv = _mas->masGetPivot(pivots, slot, mt); 
    if (piv > _mas->_last) {
        //if (piv == ULONG_MAX)
            //mas_bulk_rebalance(mas, b_node->b_end, mt);

        _slot[++b_end] = contents;
        if (!contents)
            _gap[b_end] = piv - _mas->_last + 1;
        _pivot[b_end] = piv;
    } else {
        piv = _mas->_last;
    }
    /* Appended. */
    if (piv >= _mas->_max) {
        return b_end;
    }

    do {
        /* Handle range overwrites */
        piv = _mas->masGetPivot(pivots, ++slot, mt);
    } while ((piv <= _mas->_last) && (slot <= end));

    if (piv > _mas->_last) {
        /* Copy end data to the end of the node. */
        if (slot > end) {
            _slot[++b_end] = NULL;
            _pivot[b_end] = piv;
        } else {
            copyNodeToBigNode(_mas, slot, end + 1, ++b_end);
            b_end = _b_end - 1;
        }
    }
    return b_end;
}


bool mapTreeBigNode::splitFinalNode(mapleSubtreeState *mast,int height)
{
    maple_enode ancestor;

    if (mtGetSlotsCount(_type) <= _b_end)
        return false;

    if (mtNodeIsRoot(_mas->_node,_mas->_mpTree)) {
        if (_mas->masTreeIsAlloc())
             _type = maple_node_arange_64;
        else
            _type = maple_node_range_64;
        _mas->_depth = height;
    }
    /*
        * Only a single node is used here, could be root.
        * The Big_node data should just fit in a single node.
        */
    ancestor = _mas->masNewNode(_type);
    mtSetParent(mast->l->_node, ancestor, mast->l->_offset);
    mtSetParent(mast->r->_node, ancestor, mast->r->_offset);
    mtGetNode(ancestor)->parent = _mas->masGetNode()->parent;
    mast->l->_node = ancestor;


    copyBigNodeToNode(0, (mtGetSlotsCount(_type) - 1));
    _mas->_offset = _b_end - 1;    

    return true;
}

void mapTreeBigNode::shiftRight(unsigned char shift)
{
    unsigned long size = _b_end * sizeof(unsigned long);

    memmove(_pivot + shift, _pivot, size);
    memmove(_slot + shift, _slot, size);
    memmove(_gap + shift, _gap, size);
}

void mapTreeBigNode::matAdd(struct ma_topiary *mat,
                    maple_enode dead_enode)
{
    mtSetDeadNode(dead_enode);
    mtGetTopiary(dead_enode)->next = NULL;
    if (!mat->tail) {
        mat->tail = mat->head = dead_enode;
        return;
    }

    mtGetTopiary(mat->tail)->next = dead_enode;
    mat->tail = dead_enode;
}


/*avoid split the split slot null because it belongs a range*/
int mapTreeBigNode::splitNull( unsigned char split, unsigned char slot_count)
{
    if (!_slot[split]) {
        /*
            * If the split is less than the max slot && the right side will
            * still be sufficient, then increment the split on NULL.
            */
        if ((split < slot_count - 2) &&
        (_b_end - split) > (mtGetMinSlotsCount(_type))) {
            split++;
        }else {
            split--;
        }
    }
    return split;
}

void mapTreeBigNode::setSplitParent(mapTreeState *mas,maple_enode left,
                                   maple_enode right,  unsigned char *slot,
                                   unsigned char split)
{
    //if ()
    mas_set_split_parent(mas->_node,  left, right, slot,split);
}
void mapTreeBigNode::splitData(mapleSubtreeState *mast,
        mapTreeState *mas, unsigned char split, mapTreeState *save)
{
    unsigned char p_slot;
 
    copyBigNodeToNode(0, split);
    mtSetPivot(mast->r->masGetNode(),  0,mtGetNodetype(mast->r->_node),  mast->r->_max);
    copyBigNodeToNode(split + 1, _b_end);
    mast->l->_offset = mteParentSlot(mas->_node);
    mast->l->_max = _pivot[split];
    mast->r->_min = mast->l->_max + 1;
    if (!mtNodeIsLeaf(mtGetNodetype(mas->_node))) {
        p_slot = mast->orig_l->_offset;
        setSplitParent(mast->orig_l, mast->l->_node,  mast->r->_node, &p_slot, split);
        setSplitParent(mast->orig_r, mast->l->_node,  mast->r->_node, &p_slot, split);
    } else {
        if (save->_offset > split) {
            save->_node = mast->r->_node;
            save->_min = mast->r->_min;
            save->_max = mast->r->_max;
            save->_offset -= (split + 1);
        } else {
            save->_node = mast->l->_node;
            save->_min = mast->l->_min;
            save->_max = mast->l->_max;
        }
    }
}


void mapTreeBigNode::setBigNodeEnd( mapTreeState*mas,
                                    void *entry)
{
    if (!entry)
        return;
    _slot[_b_end] = entry;    
    //if (mt_is_alloc(mas->tree))
    //    _gap[_b_end] = mas_max_gap(mas);
    int end=_b_end;
    _pivot[_b_end++] = mas->_max;    
}

void mapTreeBigNode::mastFillBnode(mapleSubtreeState *mast,
                                        mapTreeState *mas,
                                        unsigned char skip)
{
    bool cp = true;
    maple_enode old = mas->_node;
    unsigned char split, zero;

    _b_end = 0;
    if (mtGetNode(mas->_node) == mtGetNode(mas->_mpTree->ma_root)) {
        cp = false;
    } else {
        mas->masAscend();
        matAdd(mast->free, old);
        mas->_offset = mteParentSlot(mas->_node);
    }
    _min = mas->_min;

    if (cp && mast->l->_offset) {
        copyNodeToBigNode(mas, 0, (mast->l->_offset - 1), 0);
    }

    split = _b_end;
    setBigNodeEnd(mast->l, mast->l->_node);

    mast->r->_offset = _b_end;
    setBigNodeEnd(mast->r, mast->r->_node);

    if (_pivot[_b_end - 1] == mas->_max)
    cp = false;

    if (cp) {
        copyNodeToBigNode(mas,split + skip,  (mtGetSlotsCount(mtGetNodetype(_mas->_node)) - 1), _b_end);
    }
    _b_end--;

    _type = mtGetNodetype(mas->_node);

    zero = MAPLE_BIG_NODE_SLOTS - _b_end - 2;
    memset(_gap + _b_end + 1, 0,
           sizeof(unsigned long) * zero);
    memset(_slot + _b_end + 1, 0, sizeof(void*) * zero--);
    memset(_pivot + _b_end + 1, 0,
           sizeof(unsigned long) * zero);
}

bool mapTreeBigNode::mas_push_data(mapTreeState *mas, int height,
                mapleSubtreeState *mast, bool left,
                mapTreeState *save)
{
    unsigned char slot_total = _b_end;
    unsigned char end, space, split;

    mapTreeState tmp_mas(mas->_mpTree, mas->_index, mas->_last);
    tmp_mas = *mas;
    tmp_mas._depth = mast->l->_depth;

    if (left && !tmp_mas.masPrevSibling()) {
        return false;
    } else if (!left && !tmp_mas.masNextSibling()) {
    return false;
    }

    end = tmp_mas.masDataEnd();
    //show_node(tmp_mas.node); 
    slot_total += end;
    space = 2 * mtGetSlotsCount(mtGetNodetype(mas->_node)) - 2;

    /* -2 instead of -1 to ensure there isn't a triple split */
    if (mtNodeIsLeaf(_type)) {
        space--;
    }

    if (mas->_max == ULONG_MAX) {
        space--;
    }  

    if (slot_total >= space) {
        return false;
    }

    /* Get the data; Fill mast->bn */
    _b_end++;
    if (left) {
        shiftRight(end + 1);
        copyNodeToBigNode(&tmp_mas, 0, end, 0);
        _b_end = slot_total + 1;
        if (!save->_node) {
            save->_offset = mas->_offset + end + 1;
        }
    } else {
        copyNodeToBigNode(&tmp_mas, 0, end, _b_end);
    }

    /* Configure mast for splitting of mast->bn */
    split = mtGetSlotsCount(_type) - 2;
    if (left) {
        /*  Switch mas to prev node  */
        matAdd(mast->free, mas->_node);
        *mas = tmp_mas;
        /* Start using mast->l for the left side. */
        tmp_mas._node = mast->l->_node;
        *mast->l = tmp_mas;
    } else {
        matAdd(mast->free, tmp_mas._node);
        tmp_mas._node = mast->r->_node;
        *mast->r = tmp_mas;
        split = slot_total - split;
    }
    split = splitNull(split, mtGetSlotsCount(_type));
    /* Update parent slot for split calculation. */
    if (left) {
        mast->orig_l->_offset += end + 1;
    }
    splitData(mast, mas, split, save);
    mastFillBnode(mast, mas, 2);
    splitFinalNode(mast, (height + 1));

    return true;
}

bool mapTreeBigNode::middleNode(int split, unsigned char slot_count)
{
    unsigned char size = _b_end;

    if (size >= 2 * slot_count)
        return true;

    if (!_slot[split] && (size >= 2 * slot_count - 1))
        return true;

    return false;
}

int mapTreeBigNode::mabCalcSplit(mapTreeState *mas,
                                        unsigned char *mid_split)
{
    unsigned char b_end = _b_end;
    int split = b_end / 2; /* Assume equal split. */
    unsigned char min, slot_count = mtGetSlotsCount(_type);
    if ((mas->_flags & MA_STATE_BULK)){
        *mid_split = 0;
        if (mtNodeIsLeaf(_type))
            min = 2;
        else
            return (int)(b_end - mtGetMinSlotsCount(_type));

        split = b_end - min;
        mas->_flags|= MA_STATE_REBALANCE;
        if (!_slot[split])
            split--;
        return split;
    }

    if (middleNode(split, slot_count)) {
        split = b_end / 3;
        *mid_split = split * 2;
    } else {
    min = mtGetMinSlotsCount(_type);

    *mid_split = 0;
    /*
    * Avoid having a range less than the slot count unless it
    * causes one node to be deficient.
    * NOTE: mt_min_slots is 1 based, b_end and split are zero.
    */
    while (((_pivot[split] - _min) < slot_count - 1) &&
       (split < slot_count - 1) && (b_end - split > min))
        split++;
    }

    /* Avoid ending a node on a NULL entry */
    split = splitNull(split, slot_count);    
    if (!(*mid_split))
        return split;

    *mid_split = splitNull(*mid_split, slot_count);

    return split;
}

bool mapTreeBigNode::splitBigNode()
{
    mapleSubtreeState mast;
    int height = 0;
    unsigned char mid_split, split = 0;
    mapTreeState restore;

    mapTreeState l_mas(_mas->_mpTree, _mas->_index, _mas->_last);
    mapTreeState r_mas(_mas->_mpTree, _mas->_index, _mas->_last);
    mapTreeState prev_l_mas(_mas->_mpTree, _mas->_index, _mas->_last);
    mapTreeState prev_r_mas(_mas->_mpTree, _mas->_index, _mas->_last);

    MA_TOPIARY(mat, _mas->_mpTree);

    _mas->_depth = mtGetheight(_mas->_mpTree);
    /* Allocation failures will happen early. */
    if(false == _mas->masPrevAllocNode(1 + _mas->_depth * 2)) {
        return false;
    }

    mast.l = &l_mas;
    mast.r = &r_mas;
    mast.orig_l = &prev_l_mas;
    mast.orig_r = &prev_r_mas;
    mast.free = &mat;
    //mast.bn = this;
    restore._node = NULL;
    restore._offset = _mas->_offset;
    while (height++ <= _mas->_depth) {        
        if (splitFinalNode(&mast, height))
        {
            break;
        }
        l_mas = r_mas = *_mas;
        l_mas._node = _mas->masNewNode(_type);
        r_mas._node = _mas->masNewNode(_type);
        /* Try to push left. */
        if (mas_push_data(_mas, height, &mast, true, &restore))
        {      
            break;
       }        
       
        /* Try to push right. */
        if (mas_push_data(_mas, height, &mast, false, &restore))
        {      
            break;
        }
        /*find the split index*/
        split = mabCalcSplit(_mas, &mid_split); 
        splitData(&mast, _mas, split, &restore);
        /*
             * Usually correct, mab_mas_cp in the above call overwrites
             * r->max.
             */
        mast.r->_max = _mas->_max;
        mastFillBnode(&mast, _mas, 1);
        

        prev_l_mas = *mast.l;
        prev_r_mas = *mast.r;
    }
    

    /* Set the original node as dead */  
    matAdd(mast.free, _mas->_node);
    _mas->_node = l_mas._node;
    _mas->mas_replace(true);
    _mas->_offset = restore._offset;
    _mas->_min    = restore._min;
    _mas->_max    = restore._max;
    _mas->_node   = restore._node;
    return 1;
}

void mapTreeBigNode::init(void) {
    _type = mtGetNodetype(_mas->_node);    
    //copy pivot and slot to big node
    _b_end = storeBignode(_entry, _end);    
    _min   = _mas->_min;
    unsigned char zero = MAPLE_BIG_NODE_SLOTS -_b_end - 1;
    memset(_slot + _b_end + 1, 0, sizeof(void *) * zero--);
    memset(_pivot + _b_end + 1, 0,sizeof(unsigned long) * zero);
}

bool mapTreeBigNode::reuseNode(mapTreeState *mas,unsigned char end)
{
    unsigned long max;

    max = mas->_max;
    copyBigNodeToNode(0, _b_end);
    mas->_max = max;

    if (end > _b_end) {
        /* Zero end of node. */
        maple_type_t mt = mtGetNodetype(mas->_node);
        maple_node_t *mn = mas->masGetNode();
        unsigned long *pivots = mtNodePivots(mn, mt);
        void **slots = mtNodeSlots(mn, mt);
        char zero = mtGetSlotsCount(mt) - _b_end - 1;

        memset(slots + _b_end + 1, 0, sizeof(void *) * zero--);
        memset(pivots + _b_end + 1, 0, sizeof(unsigned long *) * zero);
    }
    return true;

}

int mapTreeBigNode::commitBignode(mapTreeState *mas, unsigned char end)
{
    maple_enode new_node;
    unsigned char b_end    = _b_end;
    maple_type_t b_type = _type;

    //b_end can not meet min number 
    #if 0
    if ((_b_end < mtGetMinSlotsCount[b_type]) &&
        (!mas->nodeIsRoot()) && (mas->setHeight() > 1)) {
        //return mas_rebalance(mas, b_node);
    }
     #endif
    //b_end > mt_slots[b_type],split big node into two maple node, and insert     the tree
    if (b_end >= mtGetSlotsCount(_type)) {
        return splitBigNode();
    }

    if (reuseNode(mas,end)) {
        //goto reuse_node;
        return 1;
    }

    if (false == mas->masPrevAllocNode(1)) {
        return 0;
    } 

    maple_node_t * node= (maple_node_t *)(mas->masPopNode());
    new_node = mtSetNode(node,mtGetNodetype(mas->_node));
    mtGetNode(new_node)->parent = mas->masGetNode()->parent;
    mas->_node = new_node;
    copyBigNodeToNode(0, b_end);
    mas->mas_replace(false);

//reuse_node:
    //mas_update_gap(mas);
    return 1;
}

