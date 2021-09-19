#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree.h"


static const unsigned char mt_pivots[] = {
    [maple_node_dense]       = 0,
    [maple_node_leaf_64]     = MAPLE_RANGE64_SLOTS - 1,
    [maple_node_range_64]    = MAPLE_RANGE64_SLOTS - 1,
    [maple_node_arange_64]   = MAPLE_ARANGE64_SLOTS - 1,
};

static const unsigned char mt_slots[] = {
    [maple_node_dense]       = MAPLE_NODE_SLOTS,
    [maple_node_leaf_64]     = MAPLE_RANGE64_SLOTS,
    [maple_node_range_64]    = MAPLE_RANGE64_SLOTS,
    [maple_node_arange_64]   = MAPLE_ARANGE64_SLOTS,
};

static const unsigned char mt_min_slots[] = {
    [maple_node_dense]      = MAPLE_NODE_SLOTS / 2,
    [maple_node_leaf_64]    = (MAPLE_RANGE64_SLOTS / 2) - 2,
    [maple_node_range_64]   = (MAPLE_RANGE64_SLOTS / 2) - 2,
    [maple_node_arange_64]  = (MAPLE_ARANGE64_SLOTS / 2) - 1,
};

unsigned int mtMallocNode(unsigned int number, void **slot){
    maple_node *temp   = NULL;
    unsigned int count = 0;

    for (int i=0;i<number;i++) {
        temp = (maple_node *)malloc(sizeof(maple_node) * number);
        if (temp) {
            memset((void*)temp, 0 ,(sizeof(maple_node) * number));
            slot[i] = (void *)temp;
        } else {
           break;
        }
        count++;
    }

    return count;
}

maple_type_t mtGetNodetype(maple_enode entry)
{
    return  (maple_type_t)(((unsigned long)entry >> MAPLE_ENODE_BASE) & MAPLE_NODE_TYPE_MASK);
}

unsigned char mtGetPivotsCount(maple_type_t type)
{
    return mt_pivots[type];
}

unsigned char mtGetSlotsCount(maple_type_t type)
{
    return mt_slots[type];
}

unsigned char mtGetMinSlotsCount(maple_type_t type)
{
    return mt_min_slots[type];
}

maple_enode mtSetNode(maple_node_t *node, maple_type_t type)
{
    unsigned char value = (type & 0xF);
    return (maple_enode)((unsigned long)node |((unsigned long)value<< MAPLE_ENODE_BASE));
}
maple_node_t *mtGetNode(maple_enode entry)
{
    return (maple_node_t *)((unsigned long)entry & MAPLE_NODE_MASK);
}
struct maple_topiary *mtGetTopiary(maple_enode entry)
{
    return (struct maple_topiary *)((unsigned long)entry & MAPLE_NODE_MASK);
}
void mtSetPivot(maple_node_t *mn, unsigned char piv,
                maple_type_t type, unsigned long val)
{
    switch (type) {
        default:
    case maple_node_range_64:
    case maple_node_leaf_64:
        (&mn->mr64)->pivot[piv] = val;
        break;
    case maple_node_arange_64:
        (&mn->ma64)->pivot[piv] = val;
    case maple_node_dense:
        break;
    }
}
unsigned long * mtNodePivots(maple_node_t * node, maple_type_t type)
{
    switch (type){
        case maple_node_arange_64:
            return node->ma64.pivot;
        case maple_node_range_64:
        case maple_node_leaf_64:
            return node->mr64.pivot;
        case maple_node_dense:
            return NULL;
    }

    return NULL;
}

void ** mtNodeSlots(maple_node_t *node, maple_type_t mt)
{
    switch (mt){
        case maple_node_arange_64:
            return node->ma64.slot;
        case maple_node_range_64:
        case maple_node_leaf_64:
            return node->mr64.slot;
        case maple_node_dense:
            return node->slot;
        default:
            return node->slot;
    }
}

void * mtNodeGetSlot(maple_enode enode, maple_type_t type, 
                        unsigned char offset)
{
    maple_node_t *node = mtGetNode(enode);
    void **slot  = mtNodeSlots(node, type);

    return slot[offset];
}

bool mtNodeIsDense(maple_type_t type)
{
    return type < maple_node_leaf_64;
}

bool  mtNodeIsLeaf(maple_type_t type)
{
    return type < maple_node_range_64;
}

maple_node_t * mtParent(maple_enode enode)
{
    return (maple_node_t *)((unsigned long)(mtGetNode(enode)->parent) & MAPLE_PARENT_NODE_MASK);
}

bool mtDeadNode(maple_enode enode)
{
    maple_node_t *parent, *node;

    node = mtGetNode(enode);    
   
    parent = mtParent(enode);

    return (parent == node);
}

void mtSetDeadNode(maple_enode enode)
{
    mtGetNode(enode)->parent = mtSetParentNode(enode);    
}

void mtSetParent(maple_enode enode, maple_enode parent, unsigned char slot)
{
    unsigned long val = (unsigned long) parent;
    unsigned long shift;
    unsigned long type;
    maple_type_t p_type = mtGetNodetype(parent);

    switch(p_type) {
        case maple_node_range_64:
        case maple_node_arange_64:
            shift = MAPLE_PARENT_SLOT_SHIFT;
            type  = maple_node_parent_range64;
            break;
        case maple_node_dense:
        case maple_node_leaf_64:
            shift  = type = 0;
            break;
    }
    #if 0
    type  = maple_node_arange_64;
    val &= MAPLE_NODE_MASK; /* Clear all node metadata in parent */
    val |=((unsigned long)type<< MAPLE_PARENTS_NODE_TYPE_MASK);
    val |= ((unsigned long)slot << shift) | val;
    #endif
    type = maple_node_range_64;
    val &= MAPLE_PARENT_NODE_MASK;
    val |= ((type<<1)<<MAPLE_PARENT_NODE_BASE);
    val |= ((unsigned long)slot << shift) | val;
    mtGetNode(enode)->parent = (maple_pnode)val;
}

maple_node_t * mteGetParent(maple_enode entry)
{
    return (maple_node_t *)((unsigned long)(mtGetNode(entry)->parent) & MAPLE_PARENT_NODE_MASK);
}

unsigned int mteParentSlot(maple_enode enode)
{
    unsigned long val = (unsigned long) mtGetNode(enode)->parent;

    /* Root. */
    maple_type_t type = mtGetParentType((maple_pnode)val);
    if (maple_node_arange_64 != type) {        
        return 0;
    }

    unsigned int slot = (val & ~(1<<MAPLE_PARENT_NODE_BASE-1)) >> MAPLE_PARENT_SLOT_SHIFT;
    return slot;
}
maple_type_t mteParentType(maple_pnode entry)
{
    return  (maple_type_t)(((unsigned long)entry >> MAPLE_PARENT_NODE_BASE) & MAPLE_PARENTS_NODE_TYPE_MASK);
}

maple_type_t mteParentEnum(maple_pnode p_enode)
{
    unsigned long p_type;

    p_type = mteParentType(p_enode);

    switch (p_type) {
        case maple_node_parent_range64: /* or MAPLE_PARENT_ARANGE64 */
            return maple_node_range_64;
    }
    return maple_node_dense;
}

void mas_set_split_parent(maple_enode enode,  maple_enode left,
                                   maple_enode right,  unsigned char *slot,
                                   unsigned char split)
{
    if ((*slot) <= split) {
        mtSetParent(enode, left, *slot);
    }else if (right) {
        mtSetParent(enode, right, (*slot) - split - 1);
    }

    (*slot)++;
}

