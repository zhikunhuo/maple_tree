#include <iostream>
#include <gtest/gtest.h>
#include <sys/time.h>
#include <stdlib.h>
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree_state.h"
#include "maple_tree.h"

class masAlloAndReqNumber: public testing::Test  {
protected:
    void SetUp() override {
        tree = mtMalloc();
        ms = new mapTreeState(tree,10,100);
        if (!ms) {
            EXPECT_EQ(1,0);
        }
    };

    void TearDown() override {
        free(tree);
        free(ms);
    };
    
    maple_tree_t *tree;
    mapTreeState * ms;
};

TEST_F(masAlloAndReqNumber,masGetReqAlloc){
    EXPECT_EQ(ms->masGetReqAlloc(),0);
}

TEST_F(masAlloAndReqNumber,masSetReqAlloc){
    EXPECT_EQ(ms->masGetReqAlloc(),0);    
    ms->masSetReqAlloc(10);    
    EXPECT_EQ(ms->masGetReqAlloc(),10);
    ms->masSetReqAlloc(5);   
    EXPECT_EQ(ms->masGetReqAlloc(),5);
    ms->masSetReqAlloc(100);    
    EXPECT_EQ(ms->masGetReqAlloc(),100);
    ms->masSetReqAlloc(0);    
    EXPECT_EQ(ms->masGetReqAlloc(),0);
}

TEST_F(masAlloAndReqNumber,masGetAllocated){
    EXPECT_EQ(ms->masGetReqAlloc(),0);
    EXPECT_EQ(ms->masGetAllocated(),0);
}

TEST_F(masAlloAndReqNumber,masGetAllocated_2){
    EXPECT_EQ(ms->masGetReqAlloc(),0);
    EXPECT_EQ(ms->masGetAllocated(),0);    
    ms->masSetReqAlloc(10);
    EXPECT_EQ(ms->masGetAllocated(),0);    
    ms->masSetReqAlloc(0);
    EXPECT_EQ(ms->masGetAllocated(),0);
}


