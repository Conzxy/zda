#include <type_traits>
#include <zda/darray.hpp>

#include <gtest/gtest.h>

using namespace zda;

struct A {
  A(int x_) { x = x_; }

  int x;
};

static_assert(!std::is_trivial<A>::value, "");

#define PREPARE_ARR                                                                                \
  Darray<int> arr;                                                                                 \
  for (int i = 0; i < 100; ++i) {                                                                  \
    arr.Add(i);                                                                                    \
  }

TEST(darray_test, add)
{
  Darray<int> arr;
  for (int i = 0; i < 100; ++i) {
    arr.Add(i);
    EXPECT_EQ(arr.GetBack(), i);
  }

  int i = 0;
  for (auto e : arr) {
    EXPECT_EQ(e, i);
    EXPECT_EQ(arr[i], i);
    ++i;
  }

  EXPECT_EQ(arr.size(), 100);
  EXPECT_EQ(arr.capacity(), 128);
}

TEST(darray_test, add2)
{
  Darray<A> arr;
  for (int i = 0; i < 100; ++i) {
    arr.Add(i);
    EXPECT_EQ(arr.GetBack().x, i);
  }

  int i = 0;
  for (auto e : arr) {
    EXPECT_EQ(e.x, i);
    EXPECT_EQ(arr[i].x, i);
    ++i;
  }

  EXPECT_EQ(arr.size(), 100);
  EXPECT_EQ(arr.capacity(), 128);
}

TEST(darray_test, remove)
{
  PREPARE_ARR

  int i = 99;
  while (!arr.IsEmpty()) {
    EXPECT_EQ(arr.GetBack(), i);
    arr.Remove();
    --i;
  }
}