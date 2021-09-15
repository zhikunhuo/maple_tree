#include "stdlib.h"
#include <iostream>
#include <gtest/gtest.h>
#include <sys/time.h>
#include <stdlib.h>
#include "maple_tree_utils.h"
#include "maple_tree_node.h"
#include "maple_tree_state.h"
#include "maple_tree.h"
#include "maple_tree.hpp"


class mapleTreeTest: public testing::Test  {
protected:
    void SetUp() override {
        mptree = new mapTree();
        if (!mptree) {
            EXPECT_EQ(1,0);
        }
    };

    void TearDown() override {
        delete mptree;
    };
    
    mapTree *mptree;
};

TEST_F(mapleTreeTest,showAllNodes){
    EXPECT_EQ(mptree->showAllNodes(),0);    
}

TEST_F(mapleTreeTest,insertInValid){
    EXPECT_EQ(mptree->insert(200,1,NULL),false);
    EXPECT_EQ(mptree->insert(1,200,NULL),false);
    EXPECT_EQ(mptree->insert(0,0,NULL),false);
}

TEST_F(mapleTreeTest,insertNode){
    EXPECT_EQ(mptree->insert(1,200,(void *)0x1234),true);
    EXPECT_EQ(mptree->showAllNodes(),1);
    EXPECT_EQ(mptree->insert(1,200,(void *)0x1236),false);
    EXPECT_EQ(mptree->insert(5,200,(void *)0x1236),false);
    EXPECT_EQ(mptree->insert(5,100,(void *)0x1237),false);
    EXPECT_EQ(mptree->showAllNodes(),1);
}

TEST_F(mapleTreeTest,insertNode_2){
    int i=0;
    for (i = 0; i <15;i++){
        EXPECT_EQ(mptree->insert((i*10+ 50),(i * 10 + 55),(void*)(unsigned long)(i*10+55)),true);
        EXPECT_EQ(mptree->getAllNodeCount(),(i+1));
    }
    EXPECT_EQ(mptree->showAllNodes(),i);
}

