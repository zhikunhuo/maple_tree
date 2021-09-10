#include "stdlib.h"
#include "maple_tree.h"


maple_tree_t * mtMalloc(){
    return (maple_tree_t *)calloc(1, sizeof(maple_tree_t));
}


