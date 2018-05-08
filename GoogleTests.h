#include "XorList.h"


TEST(XorList, push_back) {
    XorList<int, StackAllocator<int>> stack;
    std::list<int , StackAllocator<int>> std;
    clock_t t1, t2;
    t1 = clock();
    for (int i = 0; i < 10000; ++i) {
        stack.push_back(i);
    }
    t1 = clock() - t1;

    t2 = clock();
    for (int i = 0; i < 10000; ++i) {
        std.push_back(i);
    }
    t2 = clock() - t2;
    auto it = stack.begin();
    auto t = std.begin();
    for (int i; i < 10000; ++i) {
        EXPECT_EQ(*it, *t);
        ++it;
        ++t;
    }
    EXPECT_LE(1.5 * t1, t2);
}

TEST(XorList, push_front) {
    XorList<int, StackAllocator<int>> stack;
    std::list<int, StackAllocator<int>> std;

    clock_t t1 = clock();
    for (int i = 0; i < 10000; ++i) {
        stack.push_front(i);
    }
    t1 = clock() - t1;

    clock_t t2 = clock();
    for (int i = 0; i < 10000; ++i) {
        std.push_front(i);
    }
    t2 = clock() - t2;
    auto it = stack.begin();
    auto t = std.begin();

    for (int i = 0; i < 10000; ++i) {
        EXPECT_EQ(*it, *t);
        ++it;
        ++t;
    }
    EXPECT_LE(1.2 * t1, t2);
}

TEST(XorList, pop_back_front) {
    XorList<int, StackAllocator<int>> stack;
    std::list<int, std::allocator<int>> std;
    for (int i = 0; i < 10000; ++i) {
        stack.push_front(i);
        std.push_front(i);
    }

    for (int i = 0; i < 8000; ++i) {
        if (rand() % 2) {
            stack.pop_back();
            std.pop_back();
            EXPECT_EQ(*(--std.end()), *(--stack.end()));
        } else {
            stack.pop_front();
            std.pop_front();
            EXPECT_EQ(*stack.begin(), *std.begin());
        }
    }

    EXPECT_EQ(stack.size(), 2000);
}

TEST(XorList, insert_after_before) {
    XorList<int, StackAllocator<int>> stack;
    std::list<int, std::allocator<int>> std;
    for (int i = 0; i < 10000; ++i) {
        stack.insert_before(stack.begin(), 5);
    }

    for (int i = 0; i < 8000; ++i) {
        if (rand() % 2) {
            stack.pop_back();
            EXPECT_EQ(5, *(--stack.end()));
        }
    }
}
