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
