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
#include "maple_tree.h"


maple_tree_t * mtMalloc(void){
    return (maple_tree_t *)calloc(1, sizeof(maple_tree_t));
}

void mtInit(maple_tree_t *mt, unsigned int ma_flags)
{
    mt->ma_flags = ma_flags;
    mt->ma_root  = NULL;
}


