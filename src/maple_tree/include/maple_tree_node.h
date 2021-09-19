#ifndef __MAPLE_TREE_NODE__
#define __MAPLE_TREE_NODE__

#include "maple_tree_utils.h"
/*
 * maple_tree/maple_node.h
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

#define MAPLE_NODE_SLOTS           63
#define MAPLE_RANGE64_SLOTS        32
#define MAPLE_ARANGE64_SLOTS       21
#define MAPLE_ARANGE64_META_MAX    22
#define MAPLE_ALLOC_SLOTS          (MAPLE_NODE_SLOTS -2)

typedef struct maple_alloc {
    unsigned long       total;
    unsigned long       node_count;
    unsigned long       request_count;
    struct maple_alloc *slot[MAPLE_ALLOC_SLOTS];
}maple_alloc_t;

typedef struct maple_range_64 {
    maple_pnode         parent;
    unsigned long       pivot[MAPLE_RANGE64_SLOTS - 1];
    void *              slot[MAPLE_RANGE64_SLOTS];
}maple_range_64_t;

typedef struct maple_arange_64 {
    maple_pnode         parent;
    unsigned long       pivot[MAPLE_ARANGE64_SLOTS - 1];
    void        *       slot[MAPLE_ARANGE64_SLOTS];
    unsigned long       gap[MAPLE_ARANGE64_SLOTS];
    unsigned char       meta; 
}maple_arange_64_t;

struct maple_topiary {
	maple_pnode parent;
	maple_enode next; /* Overlaps the pivot */
};

typedef struct maple_node {
    union {
        struct {
            maple_pnode parent;
            void * slot[MAPLE_NODE_SLOTS];
        };
        maple_range_64_t    mr64;
        maple_arange_64_t   ma64;
        struct maple_alloc  alloc;
    };
}maple_node_t;


typedef enum maple_type {
    maple_node_dense,
    maple_node_leaf_64,
    maple_node_range_64,
    maple_node_arange_64,
    maple_node_parent_range64,
}maple_type_t;

#define MAPLE_ENODE_BASE        (63 -7) //for 64 system
#define MAPLE_NODE_MASK         ((1UL<<MAPLE_ENODE_BASE) -1) //
#define MAPLE_NODE_TYPE_MASK    0xF  //node type 56~59 bit

#define MAPLE_ROOT_NODE (1UL<<4) //root node flag 60


#define MAPLE_ENODE_NULL_OFFSET  5 
#define MAPLE_ENODE_NULL        (1UL << MAPLE_ENODE_NULL_OFFSET) //61 for encode null flags
#if defined(__cplusplus)
extern "C" {
#endif
unsigned int mtMallocNode(unsigned int number, void **slot);
unsigned long * mtNodePivots(maple_node_t * node, maple_type_t type);
unsigned char mtGetSlotsCount(maple_type_t type);
void ** mtNodeSlots(maple_node_t *node, maple_type_t mt);
maple_enode mtSetNode(maple_node_t *node, maple_type_t type);
maple_type_t mtGetNodetype(maple_enode entry);
maple_enode mtSetNode(maple_node_t *node, maple_type_t type);
unsigned char mtGetPivotsCount(maple_type_t type);
bool mtNodeIsDense(maple_type_t type);
bool mtNodeIsLeaf(maple_type_t type);
maple_node_t *mtGetNode(maple_enode entry);
unsigned long * masGetPivots(void);

//maple_enode mtSetRootFlag(maple_enode node);
bool mtDeadNode(maple_enode enode);
void * mtNodeGetSlot(maple_enode enode, maple_type_t type, 
                        unsigned char offset);
unsigned char mtGetMinSlotsCount(maple_type_t type);

void mtSetParent(maple_enode enode, maple_enode parent, unsigned char slot);
unsigned int mteParentSlot(maple_enode enode);
maple_node_t * mteGetParent(maple_enode entry);
maple_type_t mteParentEnum(maple_pnode p_enode);
unsigned int mteParentSlot(maple_enode enode);
maple_node_t * mtParent(maple_enode enode);
void mtSetPivot(maple_node_t *mn, unsigned char piv,
                maple_type_t type, unsigned long val);
void mas_set_split_parent(maple_enode enode,  maple_enode left,
                                   maple_enode right,  unsigned char *slot,
                                   unsigned char split);
struct maple_topiary *mtGetTopiary(maple_enode entry);
void mtSetDeadNode(maple_enode enode);

#if defined(__cplusplus)
}
#endif

#endif //__MAPLE_TREE_NODE__

