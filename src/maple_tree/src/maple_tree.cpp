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
     mt  = mtMalloc(); 
     mas = new mapTreeState(mt,0,ULONG_MAX);
}
bool mapTree::insert(unsigned long first, unsigned long end,void *entry){
    return true;
}

void mapTree::showNode(maple_enode node,int height, unsigned long *number)
{
    maple_type_t type;
    
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
                showNode(slot[loop],height,number);
            } else {
                *number++;
            }
        }
    } 
  
}

unsigned long mapTree::showAllNodes(){
    unsigned long number = 0;
    if (!mt->ma_root) {
        printf("The tree is empty\n");
    } else {
        showNode(mt->ma_root,0,&number);
    }
    return number;
}

void * mapTree::find(unsigned long *index, unsigned long max){
    return NULL;
}
