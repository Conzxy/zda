#include <gtest/gtest.h>
#include <zda/string_view.hpp>

using namespace zda;

TEST(string_view_test, to_upper)
{
    DefStringViewLiteral(view, "abcdef");
    auto upper = view.to_upper_string();
    EXPECT_EQ(upper, "ABCDEF");
}
