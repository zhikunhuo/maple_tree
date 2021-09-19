/*
 * maple_tree/maple_tree.hpp
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

#ifndef __MAPLE_TREE_BIG_NODE__
#define __MAPLE_TREE_BIG_NODE__

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "maple_tree.h"
#include "maple_tree_state.h"
#include "maple_tree.hpp"


class mapTreeBigNode {
public:
    mapTreeBigNode(mapTreeState * mas,void *entry,unsigned char end);
    void init(void);
    unsigned char storeBignode(void *entry, unsigned char end);
    void copyNodeToBigNode(mapTreeState * mas,
                                  unsigned char mas_start,
                                  unsigned char mas_end,
                                  unsigned char mab_start);
    
    void copyBigNodeToNode(unsigned char mab_start, unsigned char mab_end,mapTreeState * mas);
    bool commitBignode(mapTreeState *mas, unsigned char end);
    bool reuseNode(mapTreeState *mas,unsigned char end);
    bool splitFinalNode(mapleSubtreeState *mast,int height);
    int splitNull( unsigned char split, unsigned char slot_count);
    bool middleNode(int split, unsigned char slot_count);
    bool splitBigNode();
    void matAdd(struct ma_topiary *mat,  maple_enode dead_enode);
    void splitData(mapleSubtreeState *mast,
                   mapTreeState *mas, unsigned char split, mapTreeState *save);
    void mastFillBnode(mapleSubtreeState *mast,
                                        mapTreeState *mas,
                                        unsigned char skip);
    int mabCalcSplit(mapTreeState *mas,
                                        unsigned char *mid_split);
    bool mas_push_data(mapTreeState *mas, int height,
                mapleSubtreeState *mast, bool left,
                mapTreeState *save);
    void shiftRight(unsigned char shift);
    void setBigNodeEnd( mapTreeState*mas, void *entry);
    void setSplitParent(mapTreeState *mas,maple_enode left,
                                   maple_enode right,  unsigned char *slot,
                                   unsigned char split);
    void showBigNode();
public:
    mapTreeState * _mas;
    void        *  _entry;
    unsigned char  _end;
private:
    maple_pnode     _parent;
    maple_enode     _slot[MAPLE_BIG_NODE_SLOTS];
    unsigned long   _pivot[MAPLE_BIG_NODE_SLOTS - 1];
    unsigned long   _gap[MAPLE_BIG_NODE_SLOTS];
    unsigned long   _min;
    unsigned char   _b_end;
    enum maple_type _type;
};

struct ma_topiary {
    maple_enode    head;
    maple_enode    tail;
    maple_tree_t * mtree;
};



#define MA_TOPIARY(name, tree)      \
    struct ma_topiary name = {      \
        .head = NULL,               \
        .tail = NULL,               \
        .mtree = tree,              \
    }

#endif
