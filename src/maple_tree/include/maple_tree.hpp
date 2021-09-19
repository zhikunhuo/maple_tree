#ifndef __MAPLE_TREE_HPP__
#define __MAPLE_TREE_HPP__

#include "maple_tree_node.h"
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
 *
 */

class mapTree{
  public:
    mapTree();
    bool nodeIsRoot(maple_enode node);
    int rootExpand(void *entry);
    bool append(void *entry, unsigned long min, unsigned char end,
                        void *content);
    bool insertSlot(void *entry,
                        unsigned long min, unsigned long max,
                        unsigned char end, void *content,
                        void **slots);
    bool insert(unsigned long first, unsigned long end,void *entry);
    void showNode(maple_enode node,int height,unsigned long *count,bool flag);
    unsigned long showAllNodes();    
    void * find(unsigned long *index, unsigned long max);
    void nodeWalk(maple_type_t type,
                        unsigned long *range_min, unsigned long *range_max);
    bool mapTreeWalk(unsigned long *range_min,
                         unsigned long *range_max,  void *entry);
    unsigned long getMinPivots(unsigned long *pivots, unsigned char offset);
    unsigned long getAllNodeCount();
  private:
    bool tryStoreNodeStore( unsigned long min, unsigned long max,
                    unsigned char end, void *entry,void *content,
                    maple_type_t mt, void **slots,
                    unsigned long *pivots);
  private:
      mapTreeState * _mas;
      maple_tree_t * _mt;    
};


#endif //__MAPLE_TREE_HPP__
