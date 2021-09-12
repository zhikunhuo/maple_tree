#ifndef __MAPLE_TREE_STATE__
#define __MAPLE_TREE_STATE__

#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree.h"

#define MAS_START  ((maple_enode ) 1UL)
#define MAS_ROOT   ((maple_enode)2UL)
#define MAS_NONE   ((maple_enode )3UL)
#define MAS_PAUSE  ((maple_enode) 3UL)


#define MAPLE_STATE_ALLOC_BASE        (63 -7) //for 64 system
//The alloc flag meas that acquire maple tree node ,but has not alloced
#define MAPLE_STATE_ALLOC_FLAG        (1UL<< MAPLE_STATE_ALLOC_BASE) 
#define MAPLE_STATE_ALLOC_MASK        (1UL << MAPLE_STATE_ALLOC_BASE) //for 64 system

typedef struct ma_state {
    struct maple_tree *tree;
    unsigned long index;
    unsigned long last;
    maple_enode   node;
    unsigned long min;
    unsigned long max;
    maple_alloc_t *alloc;
    unsigned char depth;
    unsigned char offset;
    unsigned char mas_flags;
    unsigned char state;
}ma_state_t;

#define MA_STATE(name, mt, first,end)  \
    struct ma_state name = {         \
	      .tree = mt,           \
	      .index = first,     \
	      .last  = end,     \
	      .node  = MAS_START,  \
	      .min   = 0,         \
	      .max   = ULONG_MAX, \
    }

class mapTreeState{
 public:
    mapTreeState(maple_tree_t *tree, unsigned long first, unsigned long end);
    unsigned long masGetAllocated(void);
    void masSetReqAlloc(unsigned long count);
    unsigned int masGetReqAlloc();
    bool masAllocNodes();
    maple_alloc_t * masPopNode();
private:
    maple_tree_t *  _mpTree;
    unsigned long   _index;
    unsigned long   _last;
    maple_enode     _node;
    unsigned long   _min;
    unsigned long   _max;
    maple_alloc_t * _alloc;
    unsigned char   _depth;
    unsigned char   _offset;
    unsigned char   _flags;
    unsigned char   _state;    
};


#endif//__MAPLE_TREE_STATE__
