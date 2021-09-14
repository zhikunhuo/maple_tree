#include "stdlib.h"
#include "string.h"
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

maple_enode mtSetRootFlag(maple_enode node)
{
    return  (maple_enode)(((unsigned long)node | (MAPLE_ROOT_NODE)<<MAPLE_ENODE_BASE));
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

