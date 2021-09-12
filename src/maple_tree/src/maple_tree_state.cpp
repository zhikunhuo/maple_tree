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
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree_state.h"

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
        node = (maple_alloc_t *)mtMallocNode(1);
        if (!node) {
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

        count = min(requested,max_req); 
        tempNode = mtMallocNode(count);
        if (!tempNode) {
            goto nomem;
        }

        for(int i=0; i< count; i++) {
            slots[i]= &tempNode[i];
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

