#include "zda/result.hpp"

#include <gtest/gtest.h>
#include <vector>

class A {
    // A() { printf("A"); }
    A() = delete;
};

TEST (ResultTest, test)
{
    {
        zda::Result<std::vector<int>> res(false);
        EXPECT_FALSE(res);
    }

    {
        zda::Result<std::vector<int>> res(std::vector<int>({1, 2, 3}));
        EXPECT_TRUE(res);

        auto const &vec = *res;

        EXPECT_EQ(vec[0], 1);
        EXPECT_EQ(vec[1], 2);
        EXPECT_EQ(vec[2], 3);
    }

    {
        zda::Result<A> res(false);

    }
}