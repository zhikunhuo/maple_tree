/*
 * maple_tree/maple_subtree_state.cpp
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree_state.h"
#include "maple_tree.h"
#include "maple_subtree_state.hpp"


mapleSubtreeState::mapleSubtreeState()
{
    orig_l = NULL;
    orig_r  = NULL; 
    l  = NULL;
    m  = NULL;
    r  = NULL;
    free  = NULL;
    destroy = NULL;
}
