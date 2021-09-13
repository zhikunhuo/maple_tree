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
#include "maple_tree.h"
#include "maple_tree_state.h"

mapTree::mapTree()
{
     mt  = mtMalloc; 
     mas = new mapTreeState(mt,ULONG_MAX);
}