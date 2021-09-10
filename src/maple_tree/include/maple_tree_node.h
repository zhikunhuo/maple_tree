#ifndef __MAPLE_TREE_NODE__
#define __MAPLE_TREE_NODE__
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
    struct maple_pnode *parent;
    unsigned long       pivot[MAPLE_RANGE64_SLOTS - 1];
    void *              slot[MAPLE_RANGE64_SLOTS];
}maple_range_64_t;

typedef struct maple_arange_64 {
    struct maple_pnode *parent;
    unsigned long       pivot[MAPLE_ARANGE64_SLOTS - 1];
    void        *       slot[MAPLE_ARANGE64_SLOTS];
    unsigned long       gap[MAPLE_ARANGE64_SLOTS];
    unsigned char       meta; 
}maple_arange_64_t;


typedef struct maple_node {
    union {
	struct {
	    struct maple_pnode *parent;
	    void * slot[MAPLE_NODE_SLOTS];
	};
	maple_range_64_t    mr64;
    maple_arange_64_t   ma64;
    struct maple_alloc  alloc;
    };
}maple_node_t;


#define MAPLE_ENODE_BASE        (63 -7) //for 64 system
#define MAPLE_NODE_MASK         ((1UL<<MAPLE_ENODE_BASE) -1) //
#define MAPLE_NODE_TYPE_MASK    0xF  //node type 56~59 bit

#define MAPLE_ENODE_NULL_OFFSET  5 
#define MAPLE_ENODE_NULL        (1UL << MAPLE_ENODE_NULL_OFFSET) //61 for encode null flags


#endif //__MAPLE_TREE_NODE__
