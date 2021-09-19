#ifndef __MAPLE_TREE__
#define __MAPLE_TREE__

#include "maple_tree_node.h"
/*
 * maple_tree/maple_tree.h
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

#define MAPLE_ALLOC_RANGE	0x01
#define MAPLE_USE_RCU		0x02
#define	MAPLE_HEIGHT_OFFSET	0x02
#define	MAPLE_HEIGHT_MASK	0x7C

typedef struct maple_tree {
    unsigned int         ma_flags;
    unsigned long        tree_elements;
    maple_enode          ma_root;
}maple_tree_t;

#if defined(__cplusplus)
extern "C" {
#endif
    maple_tree_t * mtMalloc(void);
    void mtSetHeight(maple_tree_t *mt, unsigned char depth);
    unsigned int mtGetheight(maple_tree_t *mt);
    bool mtNodeIsRoot(maple_enode node, maple_tree_t *mt);

#if defined(__cplusplus)
}
#endif

#define MAPLE_PARENT_NODE_BASE (63 -8)
#define MAPLE_PARENTS_NODE_TYPE_MASK    0x7  //node type 56~59 bit
#define MAPLE_PARENT_SLOT_SHIFT  (63-4) //59~63  slot id
#define MAPLE_PARENT_NODE_MASK  ((1UL<<MAPLE_PARENT_NODE_BASE) -1)

typedef enum maple_parent_type {
    maple_node_root = 1,
}maple_parent_type_t;

#define MA_STATE_BULK		1
#define MA_STATE_REBALANCE	2

#define MAPLE_ALLOC_RANGE	0x01
#define MAPLE_USE_RCU		0x02
#define	MAPLE_HEIGHT_OFFSET	0x02
#define	MAPLE_HEIGHT_MASK	0x7C

#define set_parent_root(x) (maple_pnode)((unsigned long)x|(unsigned long)(maple_node_root & MAPLE_PARENTS_NODE_TYPE_MASK)<<MAPLE_PARENT_NODE_BASE)

#define MAPLE_BIG_NODE_SLOTS	(MAPLE_RANGE64_SLOTS * 2 + 2)

struct maple_big_node {
	maple_pnode parent;
	maple_enode slot[MAPLE_BIG_NODE_SLOTS];
	unsigned long pivot[MAPLE_BIG_NODE_SLOTS - 1];
	unsigned long gap[MAPLE_BIG_NODE_SLOTS];
	unsigned long min;
	unsigned char b_end;
	enum maple_type type;
};



#endif //__MAPLE_TREE__
