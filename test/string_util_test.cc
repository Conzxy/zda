//
// Created by conzxy on 23-7-29.
//
#define ALLOW_STRING_UTIL_TMP
#include "zda/string_util.hpp"

#include <gtest/gtest.h>

using namespace zda;
using namespace zda::literal;

TEST(StringUtilTest, normal)
{
    auto str = str_cat("1", StringViewLiteral("2"), std::string("3"), '4');
    EXPECT_EQ(str, "1234");

    auto str2 = str_cat('1', std::string("2"), StringViewLiteral("3"), "4");
    EXPECT_EQ(str2, "1234");
}

TEST(StringUtilTest, fmt)
{
    std::string s = "2";
    auto str = str_catf("%s%S%v%c", "1", &s, StringViewLiteral("3"), '4');
    EXPECT_EQ(str, "1234");

    str_appendf(&str, "5678%c%s%v", '9', "10", StringViewLiteral("11"));
    EXPECT_EQ(str, "1234567891011");

    char buf[4096];

    buf_catf(buf, sizeof(buf), "1234%v%S%v%c", "1"_sv, &s,
             StringViewLiteral("3"), '4');
    EXPECT_EQ(strcmp(buf, "12341234"), 0);
}