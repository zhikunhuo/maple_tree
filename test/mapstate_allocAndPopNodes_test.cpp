#include "stdlib.h"
#include <iostream>
#include <gtest/gtest.h>
#include <sys/time.h>
#include <stdlib.h>
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree_state.h"
#include "maple_tree.h"

class masAlloAndPopNodes: public testing::Test  {
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

TEST_F(masAlloAndPopNodes,masAllocNodes){
    EXPECT_EQ(ms->masGetReqAlloc(),0);    
    ms->masSetReqAlloc(1);    
    EXPECT_EQ(ms->masGetReqAlloc(),1);
    EXPECT_EQ(ms->masAllocNodes(),true);
    EXPECT_EQ(ms->masGetAllocated(),1);    
    EXPECT_EQ(ms->masGetReqAlloc(),0);    
    maple_alloc_t * nodes = ms->masPopNode();
    EXPECT_NE(nodes,nullptr);
    free(nodes);
    EXPECT_EQ(ms->masGetAllocated(),0);
    EXPECT_EQ(ms->masGetReqAlloc(),0);  
}

TEST_F(masAlloAndPopNodes,masAllocNodes_2){
    EXPECT_EQ(ms->masGetReqAlloc(),0);    
    ms->masSetReqAlloc(1);    
    EXPECT_EQ(ms->masGetReqAlloc(),1);
    EXPECT_EQ(ms->masAllocNodes(),true);
    EXPECT_EQ(ms->masGetAllocated(),1);    
    EXPECT_EQ(ms->masGetReqAlloc(),0);    
    maple_alloc_t * nodes = ms->masPopNode();
    EXPECT_NE(nodes,nullptr);
    free(nodes);
    EXPECT_EQ(ms->masGetAllocated(),0);
    EXPECT_EQ(ms->masGetReqAlloc(),0);  
}

TEST_F(masAlloAndPopNodes,masPrevAllocNode){
    EXPECT_EQ(ms->masGetReqAlloc(),0);    
    ms->masPrevAllocNode(1);
    EXPECT_EQ(ms->masGetReqAlloc(),0);   
    EXPECT_EQ(ms->masGetAllocated(),1);
    ms->masPrevAllocNode(5);     
    EXPECT_EQ(ms->masGetReqAlloc(),0);   
    EXPECT_EQ(ms->masGetAllocated(),5);
    ms->masPrevAllocNode(10);     
    EXPECT_EQ(ms->masGetReqAlloc(),0);   
    EXPECT_EQ(ms->masGetAllocated(),10);
    ms->masPrevAllocNode(4);     
    EXPECT_EQ(ms->masGetReqAlloc(),0);   
    EXPECT_EQ(ms->masGetAllocated(),10);    
    maple_alloc_t * tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),9);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),8);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),7);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),6);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),5);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),4);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),3);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),2);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),1);
    tempNodes = ms->masPopNode();
    EXPECT_NE(tempNodes,nullptr);
    EXPECT_EQ(ms->masGetAllocated(),0);
    EXPECT_EQ(ms->masGetReqAlloc(),0);
    tempNodes = ms->masPopNode();
    EXPECT_EQ(tempNodes,nullptr);
}
