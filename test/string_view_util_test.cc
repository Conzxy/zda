#include "zda/string_view_util.hpp"

#include <gtest/gtest.h>

using namespace zda;

TEST(StringViewUtilTest, converter) {
    {
        auto res = string_view_to_s64(StringViewLiteral("32"));
        ASSERT_TRUE(res);
        EXPECT_EQ(*res, 32);
    }
    {
        auto res = string_view_to_s64(StringViewLiteral("s64"));
        ASSERT_FALSE(res);
    }
}