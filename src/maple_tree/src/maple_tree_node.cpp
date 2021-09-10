#include "stdlib.h"
#include "maple_tree_node.h"


maple_node * mtMallocNode(unsigned int number){
    return  (maple_node *)cmalloc(number, sizeof(maple_node));
}
