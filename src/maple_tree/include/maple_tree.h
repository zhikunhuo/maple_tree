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

#if defined(__cplusplus)
}
#endif

#define MAPLE_PARENT_NODE_BASE (63 -8)

#define set_parent_ptr(x) (struct maple_pnode *)((unsigned long)x |((MA_ROOT_PARENT) <<MAPLE_PARENT_NODE_BASE))

#endif //__MAPLE_TREE__
