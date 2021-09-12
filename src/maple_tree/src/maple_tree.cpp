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



