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
    bool insert(unsigned long first, unsigned long end,void *entry);
    bool showAllNodes();    
    void * find(unsigned long *index, unsigned long max);
  private:
      mapTreeState *mas;
      maple_tree_t *mt;    
};


#endif //__MAPLE_TREE_HPP__
