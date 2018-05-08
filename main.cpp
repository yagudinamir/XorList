#include "gtest/gtest.h"
#include "GoogleTests.h"


int main(int args, char** argv) {
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
    /*XorList<int, StackAllocator<int>> stack;
    std::list<int , StackAllocator<int>> std;
    clock_t t1, t2;
    std::cout << "push_back:\n";
    t1 = clock();
    for (int i = 0; i < 10000; ++i) {
        stack.push_back(i);
        //std::cout << *(--stack.end()) << " ";
    }
    t1 = clock() - t1;
    //std::cout << std::endl;
    t2 = clock();
    for (int i = 0; i < 10000; ++i) {
        std.push_back(i);
        //std::cout << *(--std.end()) << " ";
    }
    t2 = clock() - t2;
    for (auto it = stack.begin(); it != stack.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    for (auto it = std.begin(); it != std.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n" <<  t1 << " " << t2 << "\n";

    std::cout << "push_front:\n";
    t1 = clock();
    for (int i = 0; i < 100; ++i) {
        stack.push_front(i);
        //std::cout << *stack.begin() << " ";
    }
    t1 = clock() - t1;
    //std::cout << std::endl;
    t2 = clock();
    for (int i = 0; i < 100; ++i) {
        std.push_front(i);
        //std::cout << *std.begin() << " ";
    }
    t2 = clock() - t2;
    for (auto it = stack.begin(); it != stack.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    for (auto it = std.begin(); it != std.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n" << t1 << " " << t2 << "\n";

    std::cout << "pop_back:\n";
    t1 = clock();
    for (int i = 0; i < 1000; ++i) {
        stack.pop_back();
    }
    t1 = clock() - t1;
    t2 = clock();
    for (int i = 0; i < 1000; ++i) {
        std.pop_back();
    }
    t2 = clock() - t2;
    std::cout << t1 << " " << t2 << "\n";

    std::cout << "pop_front:\n";
    t1 = clock();
    for (int i = 0; i < 1000; ++i) {
        stack.pop_front();
    }
    t1 = clock() - t1;
    t2 = clock();
    for (int i = 0; i < 1000; ++i) {
        std.pop_front();
    }
    t2 = clock() - t2;
    std::cout << t1 << " " << t2 << "\n";

    std::cout << "Size after pop_backs && pop_fronts" << std::endl << stack.size() << " " << std.size() << std::endl;
    return 0; */
}