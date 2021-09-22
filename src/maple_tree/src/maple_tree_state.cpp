/*
 * maple_tree/maple_tree_state.h
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree_state.h"
#include "maple_tree.h"

mapTreeState::mapTreeState()
{
    _mpTree = NULL;
    _index  = 0;
    _last   = 0;
    _node   = 0;
    _min    = 0;
    _max    = 0;
    _alloc  = 0;
    _depth  = 0;
    _offset = 0;

}

mapTreeState::mapTreeState(maple_tree_t *tree, unsigned long first, unsigned long end){
    _mpTree = tree;
    _index  = first;
    _last   = end;
    _node   = MAS_START;
    _min    = 0;
    _max    = ULONG_MAX;
    _alloc  = NULL;
    _depth  = 0;
    _offset = 0;
    _flags  = 0;
}

mapTreeState& mapTreeState::operator=(const mapTreeState& mpState){
    if (this != & mpState) {
        _mpTree = mpState._mpTree;
        _index  = mpState._index;
        _last   = mpState._last;
        _node   = mpState._node;
        _min    = mpState._min;
        _max    = mpState._max;
        _alloc  = mpState._alloc;
        _depth  = mpState._depth;
        _offset = mpState._offset;
        _flags  = mpState._flags;
        _state  = mpState._state;
    }

    return *this;
}

unsigned long mapTreeState::masGetAllocated(void) {
    if (!_alloc || ((unsigned long)_alloc & MAPLE_STATE_ALLOC_MASK)) {
        return 0;
    }

    return _alloc->total;
}

void mapTreeState::masSetReqAlloc(unsigned long count)
{
    if (!_alloc || ((unsigned long)_alloc & MAPLE_STATE_ALLOC_MASK)) {
        if (!count) {
            _alloc = NULL;
        } else {
            _alloc = (maple_alloc_t *)((count) | MAPLE_STATE_ALLOC_FLAG);
        }

        return ;
    }

    _alloc->request_count = count;
}
unsigned int mapTreeState::masGetReqAlloc()
{
    if ((unsigned long)_alloc & MAPLE_STATE_ALLOC_FLAG) {
        return ((unsigned long)_alloc & ~MAPLE_STATE_ALLOC_MASK);
    } else if(_alloc) {
        return _alloc->request_count;
    }

    return 0;
}

maple_enode mapTreeState::masNewNode(maple_type type)
{
    return mtSetNode((maple_node_t *)(masPopNode()), type);
}

bool mapTreeState::masAllocNodes()
{
    maple_alloc_t * node;
    maple_alloc_t ** nodep = &_alloc;

    unsigned long allocated = masGetAllocated();
    unsigned long success   = allocated;
    unsigned long requested = masGetReqAlloc();
    unsigned long count;

    if (!requested) {
        return false;
    }

    masSetReqAlloc(0);

    if (!allocated || _alloc->node_count == (MAPLE_ALLOC_SLOTS - 1)) {
        if (!mtMallocNode(1,(void **)&node)) {
             goto nomem;
        }
       
        if (allocated) {
            node->slot[0] = _alloc;
        }

        success++;
        _alloc = node;
        requested--;
    }

    node  = _alloc;
    while(requested) {
        void **slots         = (void **)&node->slot;
        maple_node *tempNode = NULL;
        unsigned int max_req = MAPLE_NODE_SLOTS - 1;
        if (node->slot[0]) {
            unsigned int offset = node->node_count + 1;
            slots = (void **)&node->slot[offset];
            max_req -= offset;
        }

        count = mtMallocNode(min(requested,max_req), slots);
        if (!count) {
            goto nomem;
        }
        node->node_count += count;
        if (slots == (void **)&node->slot) {
            node->node_count--;
        }

        success +=count;
        nodep = &node->slot[0];
        node = *nodep;
        requested -= count;
    }

    _alloc->total = success;
    return true;
nomem:
    masSetReqAlloc(requested);    
    if (_alloc && !((unsigned long)_alloc & MAPLE_STATE_ALLOC_FLAG)) {
        _alloc->total = success;
    }

    return false;
    
}

maple_alloc_t * mapTreeState::masPopNode()
{
    maple_alloc_t *ret  = NULL;
    maple_alloc_t *node = _alloc;
    unsigned long total =  masGetAllocated();

    if (!total) {
        return NULL;
    }

    if (1 == total) {
        _alloc = NULL;
        ret = node;
        goto single_node;
    }

    if (!node->node_count) {
        _alloc = node->slot[0];
        node->slot[0] = NULL;
        _alloc->total = node->total -1 ;
        ret = node;
        goto new_head;
    }

    node->total--;
    ret = node->slot[node->node_count];
    node->slot[node->node_count--] = NULL;
single_node:
new_head:
    ret->total = 0;
    ret->node_count = 0;
    if (ret->request_count) {
        masSetReqAlloc(ret->request_count + 1);
        ret->request_count = 0;
    }

    return ret;
}

bool mapTreeState::masPrevAllocNode(int count)
{
    unsigned long allocated = masGetAllocated();

    if (allocated < count ) {
        masSetReqAlloc((count-allocated));
        return masAllocNodes();
    }

    return true;
}

bool mapTreeState::masIsStart(void)
{
    return (_node == MAS_START);
}
bool mapTreeState::masIsNone(void)
{
    return (_node == MAS_NONE);
}

bool mapTreeState::masStart()
{
    if (masIsStart()) {        
        _node   = MAS_NONE;
        _min    = 0;
        _max    = ULONG_MAX;
        _depth  = 0;
        _offset = 0;

        if(!_mpTree->ma_root){
            return true;
        }

        _node = _mpTree->ma_root;
    }

    return false;
}
void mapTreeState::masSetRange(unsigned long first, unsigned long end)
{
    _index = first;
    _last  = end;
}

void mapTreeState::masSetNode(maple_enode node){
    _node = node;
}

unsigned long mapTreeState::masGetIndex(void){
    return _index;
}

void mapTreeState::masSetIndex(unsigned long index){
    _index =  index;
}

unsigned char mapTreeState::masGetOffset(void){
    return _offset;
}

void mapTreeState::masSetOffset(unsigned char offset) {
    _offset = offset;
}
maple_node_t * mapTreeState::masGetNode(void)
{
    return mtGetNode(_node);
}
unsigned long * mapTreeState::masGetPivots(void)
{
    return mtNodePivots(masGetNode(),masGetNodeType());
}

maple_type_t mapTreeState::masGetNodeType(void)
{
    return mtGetNodetype(_node);
}

void ** mapTreeState::masNodeSlots(void)
{
    return mtNodeSlots(masGetNode(), masGetNodeType());
}

bool mapTreeState::masIsSpanWsr(unsigned long piv,
                                    maple_type_t type, void *entry)
{
    unsigned long max;
    unsigned long last = _last;

    /* Contained in this pivot */
    if (piv > last) {
         return false;
    }

    max = _max;
    if (mtNodeIsLeaf(type)) {
        /* Fits in the node, but may span slots. */
        if (last < max) {
            return false;
        }

        /* Writes to the end of the node but not null. */
        if ((last == max) && entry) {
            return false;
        }

        /*
             * Writing ULONG_MAX is not a spanning write regardless of the
             * value being written as long as the range fits in the node.
             */
        if ((last == ULONG_MAX) && (last == max)) {
            return false;
        }
    } else if ((piv == last) && entry) {
        return false;
    }

    return true;
}

unsigned char mapTreeState::masDataEnd()
{
    maple_type_t type;
    unsigned char offset;
    unsigned long *pivots;

    type = mtGetNodetype(_node);
    #if 0
    if (type == maple_node_arange_64) {
        return ma_meta_end(mte_to_node(mas->node), type);
    }
    #endif
    offset = mtGetMinSlotsCount(type);
    pivots = mtNodePivots(masGetNode(), type);
    if ((!pivots[offset])) {
        goto decrement;
    }

    /* Higher than the min. */
    offset = mtGetPivotsCount(type) - 1;
    /* Check exceptions outside of the loop. */
    if ((pivots[offset])) {
        /* At least almost full. */

        /* Totally full. */
        if (pivots[offset] != _max) {
            return offset + 1;
        }
        return offset;
    }

decrement:
    while (--offset) {
        if ((pivots[offset]))
            break;
    };
    if ((pivots[offset] < _max)) {
        offset++;
    }
    return offset;
}

unsigned long mapTreeState::masGetPivot(unsigned long *pivots,
                                            unsigned char piv,
                                            maple_type_t type)
{
    if (piv >= mtGetPivotsCount(type))
        return _max;

    return pivots[piv];
}

unsigned long mapTreeState::masGetLogicalPivot(unsigned long *pivots,
            unsigned char offset, maple_type_t type)
{
    unsigned long lpiv = masGetPivot(pivots, offset, type);

    if (!lpiv && offset) {
        return _max;
    }

    return lpiv;
}

bool mapTreeState::masTreeIsAlloc()
{
    return (_mpTree->ma_flags & MAPLE_ALLOC_RANGE);
}

void mapTreeState::masCopyNodeToBig(unsigned char mas_start,
            unsigned char mas_end, struct maple_big_node *b_node,
            unsigned char mab_start)
{
    maple_type_t mt;
    maple_node_t *node = masGetNode();
    void  **slots;
    unsigned long *pivots, *gaps;
    int i = mas_start, j = mab_start;
    unsigned char piv_end;

    mt = mtGetNodetype(_node);
    pivots = mtNodePivots(node, mt);
    if (!i) {
        b_node->pivot[j] = pivots[i++];
        if (i > mas_end)
            goto complete;
        j++;
    }

    piv_end = min(mas_end, mtGetPivotsCount(mt));
    for (; i < piv_end; i++, j++) {
        b_node->pivot[j] = pivots[i];
        if (!b_node->pivot[j])
            break;

        if (_max == b_node->pivot[j])
            goto complete;
    }

    if (i <= mas_end)
        b_node->pivot[j] = masGetPivot(pivots, i, mt);

complete: 
    b_node->b_end = ++j;
    j -= mab_start;
    slots = mtNodeSlots(node, mt);
    memcpy(b_node->slot + mab_start, slots + mas_start, sizeof(void *) * j);
    if (!mtNodeIsLeaf(mt) &&    masTreeIsAlloc()) {
        //gaps = ma_gaps(node, mt);
        //memcpy(b_node->gap + mab_start, gaps + mas_start,
        //       sizeof(unsigned long) * j);
    }
}


bool mapTreeState::nodeIsRoot(void){
    return mtNodeIsRoot(_node, _mpTree);
}

int mapTreeState::masAscend(void)
{
    maple_pnode p_enode; /* parent enode. */
    maple_enode a_enode; /* ancestor enode. */
    maple_node_t *a_node; /* ancestor node. */
    maple_node_t *p_node; /* parent node. */
    unsigned char a_slot;
    maple_type_t a_type;
    unsigned long min, max;
    unsigned long *pivots;
    unsigned char offset;
    bool set_max = false, set_min = false;

    a_node = masGetNode();
    if (nodeIsRoot()) {
        _offset = 0;
        return 0;
    }

    p_enode = masGetNode()->parent;
    #if 0
    if (ma_is_root(p_enode, mas->tree)) {
        mas->node = 
        mas->max = ULONG_MAX;
        mas->min = 0;
        return 0;
    }
    #endif
    p_node  = mteGetParent(_node);
    a_type  = mteParentEnum(p_enode);
    offset  = mteParentSlot(_node);
    a_enode = mtSetNode(p_node, a_type);

    if (mtDeadNode(_node)) {
        return 1;
    }
    
    /* Check to make sure all parent information is still accurate */
    #if 0
    if (p_node != mte_parent(mas->node)) {
        return 1;
    }
    #endif
     
    _node = a_enode;
    _offset = offset;
    if (mtNodeIsRoot(p_enode, _mpTree)) {      
        //_node = mtSetRootFlag(a_enode);
        _max = ULONG_MAX;
        _min = 0;
        return 0;
    }
    
    min = 0;
    max = ULONG_MAX;
    pivots = mtNodePivots(a_node, a_type);
    a_slot = offset;
    do {

        if (!set_min && a_slot) {
            set_min = true;
            min = pivots[a_slot - 1] + 1;
        }

        if (!set_max && a_slot < mtGetPivotsCount(a_type)) {
            set_max = true;
            max = pivots[a_slot];
        }
        if (mtDeadNode(a_enode)) {
            return 1;
        }

        if (mtNodeIsRoot(p_node, _mpTree)) {
            break;
        }
             
        p_node = mteGetParent(a_enode);
        pivots = mtNodePivots(a_node, a_type);
        a_slot = mteParentSlot(p_enode);
        p_enode = mtGetNode(a_enode)->parent;
        a_enode = mtSetNode(p_node, a_type);    
    }while (!set_min || !set_max);
    
    _max = max;
    _min = min;
    return 0;
}


void mapTreeState::masDescend()
{
    maple_type_t type;
    unsigned long *pivots;
    maple_node_t *node;
    void **slots;

    node   = masGetNode();
    type   = mtGetNodetype(_node);
    pivots = mtNodePivots(node, type);
    slots  = mtNodeSlots(node, type);

    if (_offset) {
        _min = pivots[_offset - 1] + 1;
    }
    _max = masGetPivot(pivots, _offset, type);
    _node = slots[_offset];
}

bool mapTreeState::masPrevSibling()
{
    unsigned int p_slot = mteParentSlot(_node);

    if (nodeIsRoot()) {
        return false;
    }

    if (!p_slot) {
        return false;
    }

    masAscend();
    _offset = p_slot - 1;
    masDescend();
    return true;
}

maple_enode mapTreeState::masGetSlot(unsigned char offset)
{
    return mtNodeGetSlot(_node, mtGetNodetype(_node), offset);
}

bool mapTreeState::masNextSibling()
{
    unsigned char end;
    mapTreeState parent=*this;

    if (nodeIsRoot()) {
        return false;
    }

    parent.masAscend();
    end = parent.masDataEnd();
    parent._offset = mteParentSlot(_node) + 1;
    if (parent._offset > end) {
        return false;
    }

    if (!parent.masGetSlot(parent._offset)) {
        return false;
    }

    *this = parent;
    parent.masDescend();
 
    return true;
}

void mapTreeState::mas_replace(bool advanced)
{
    maple_node_t *mn = masGetNode();
    maple_enode old_enode;
    unsigned char offset = 0;
    void **slots = NULL;


    if (nodeIsRoot()) {
        old_enode = _mpTree->ma_root;
    } else {
        offset = mteParentSlot(_node);
        slots = mtNodeSlots(mtParent(_node), mteParentEnum(_node));
        old_enode = slots[offset];
    }


    if (!advanced && !mtNodeIsLeaf(mtGetNodetype(_node))) {
        //mas_adopt_children(mas, mas->node);
    }

    if (nodeIsRoot()) {
        mn->parent = mtSetParentRootNode(_mpTree);
        _mpTree->ma_root = _node;
        mtSetHeight(_mpTree, _depth);
    } else {
        slots[offset] = _node;
    }

    //if (!advanced)
    //	mas_free(mas, old_enode);
}
void mapTreeState::setHeight()
{
    mtSetHeight(_mpTree, _depth);
}

void mapTreeState::showNode(void){    
    maple_type_t mt         = mtGetNodetype(_node);
    maple_node_t *node      = mtGetNode(_node);
    void **          slots  = mtNodeSlots(node, mt);
    unsigned long *  piots  = mtNodePivots(node, mt);

    printf("begin node:%p,type:%d\n",node,mt);
    for(int loop=0; loop <mtGetPivotsCount(mt);loop++) {
        printf("\tloop: index:%d, pivots:%lu,slots:%p\n",loop,piots[loop],slots[loop]);
    }
    printf("end node:%p,type:%d\n",node,mt);
}

