#include "gtest/gtest.h"
#include "GoogleTests.h"


int main(int args, char** argv) {
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
