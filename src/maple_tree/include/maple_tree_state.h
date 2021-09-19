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
    mapTreeState();
    mapTreeState(maple_tree_t *tree, unsigned long first, unsigned long end);
    unsigned long masGetAllocated(void);
    void masSetReqAlloc(unsigned long count);
    maple_enode masNewNode(maple_type type);
    unsigned int masGetReqAlloc();
    bool masAllocNodes();
    maple_alloc_t * masPopNode();    
    bool masPrevAllocNode(int count);
    bool masIsStart(void);    
    bool masIsNone(void);
    bool masStart();
    void masSetNode(maple_enode node);
    maple_node_t * masGetNode(void);
    unsigned long * masGetPivots(void);
    maple_type_t masGetNodeType(void);
    void ** masNodeSlots(void);
    unsigned long masGetIndex(void);
    void masSetIndex(unsigned long index);
    unsigned char masGetOffset(void);
    void masSetOffset(unsigned char offset);
    void masSetRange(unsigned long first, unsigned long end);
    bool masIsSpanWsr(unsigned long piv, maple_type_t type, void *entry);
    unsigned char masDataEnd();
    unsigned long masGetPivot(unsigned long *pivots,
                                   unsigned char piv,
                                   maple_type_t type);
    unsigned long masGetLogicalPivot(unsigned long *pivots,
                                            unsigned char offset,
                                            maple_type_t type);
    bool masTreeIsAlloc();
    void masCopyNodeToBig(unsigned char mas_start,
            unsigned char mas_end, struct maple_big_node *b_node,
            unsigned char mab_start);
    int  masAscend(void);
    void masDescend();
    bool masPrevSibling();    
    maple_enode masGetSlot(unsigned char offset);
    bool masNextSibling();
    void setHeight();
    bool nodeIsRoot(void);
    void mas_replace(bool advanced);
    void showNode(void);
    mapTreeState& operator=(const mapTreeState& mpState);
public:
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
