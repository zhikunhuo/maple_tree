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

typedef struct maple_enode *maple_enode;
typedef struct maple_pnode *maple_pnode;


#define MAPLE_ALLOC_RANGE	0x01
#define MAPLE_USE_RCU		0x02
#define	MAPLE_HEIGHT_OFFSET	0x02
#define	MAPLE_HEIGHT_MASK	0x7C

typedef struct maple_tree {
    unsigned int        ma_flags;
    unsigned long       tree_elements;
    maple_enode        *ma_root;
}maple_tree_t;


#endif //__MAPLE_TREE__
