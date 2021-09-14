#ifndef __MAPLE_TREE_UTILS__
#define __MAPLE_TREE_UTILS__


#define ULONG_MAX (~0UL)

#define min(a,b)     (((a) <(b))?(a):(b))

#ifndef __cplusplus
#ifndef bool
    typedef enum __bool {
        false = 0,
        true  = 1
    }bool;
    //typedef unsigned int bool;
#endif
#endif

typedef void * maple_enode;
typedef void * maple_pnode;

#endif//__MAPLE_TREE_UTILS__
