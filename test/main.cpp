#include <stdio.h>
#include <gtest/gtest.h>
#include <stddef.h>

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
