/*
 * maple_tree/maple_tree.c
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
#include "string.h"
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree.h"

maple_tree_t * mtMalloc(void){
    return (maple_tree_t *)calloc(1, sizeof(maple_tree_t));
}

void mtInit(maple_tree_t *mt, unsigned int ma_flags)
{
    mt->ma_flags = ma_flags;
    mt->ma_root  = NULL;
}

void mtSetHeight(maple_tree_t *mt, unsigned char depth)
{
    unsigned int new_flags = mt->ma_flags;

    new_flags &= ~MAPLE_HEIGHT_MASK;
    new_flags |= depth << MAPLE_HEIGHT_OFFSET;
    mt->ma_flags = new_flags;
}

unsigned int mtGetheight(maple_tree_t *mt)
{
	return (mt->ma_flags & MAPLE_HEIGHT_MASK) >> MAPLE_HEIGHT_OFFSET;
}

bool mtNodeIsRoot(maple_enode node, maple_tree_t *mt)
{
	return (mtGetParentNode(mtGetNode(node)->parent) == (maple_node_t*)mt)?true:false;
}

maple_pnode mtSetParentRootNode(maple_tree_t *mt)
{
    return (maple_pnode)((unsigned long)mt|(unsigned long)((MAPLE_PARENT_NODE<<MAPLE_PARENT_NODE_BASE)));
}

maple_pnode mtSetParentNode(maple_enode enode)
{
    return (maple_pnode)((unsigned long)enode & MAPLE_PARENT_NODE_MASK);
}

maple_node_t * mtGetParentNode(maple_pnode enode)
{
    return (maple_node_t *)((unsigned long)enode & MAPLE_PARENT_NODE_MASK);
}

maple_type_t mtGetParentType(maple_pnode enode)
{
    return (maple_type_t)((((unsigned long)enode>>MAPLE_PARENT_NODE_BASE) & MAPLE_PARENTS_NODE_TYPE_MASK) >> 1);
}

maple_enode mtSetParentRoot(maple_enode node)
{
    return  (maple_enode)(((unsigned long)node & ~MAPLE_PARENT_NODE_FLAG_MASK) | MAPLE_PARENT_NODE_FLAG_MASK);
}
