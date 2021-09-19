/*
 * maple_tree/maple_subtree_state.hpp
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

#ifndef __MAPLE_SUBTREE_STATE__
#define __MAPLE_SUBTREE_STATE__

class mapleSubtreeState {
public:
    mapleSubtreeState();
public:
    mapTreeState *orig_l;    /* Original left side of subtree */
    mapTreeState *orig_r;    /* Original right side of subtree */
    mapTreeState *l;     /* New left side of subtree */
    mapTreeState *m;     /* New middle of subtree (rare) */
    mapTreeState *r;         /* New right side of subtree */
    struct ma_topiary *free;        /* nodes to be freed */
    struct ma_topiary *destroy; /* Nodes to be destroyed (walked and freed) */
    //mapTreeBigNode *bn;
};
#endif
