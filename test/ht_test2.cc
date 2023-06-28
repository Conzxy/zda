#include "zda/ht.h"
#include <zda/ht.hpp>

#include <gtest/gtest.h>

using namespace zda;

struct A {
  A(int x_)
    : x(x_)
  {
  }

  A(A const &rhs)
  noexcept
    : x(rhs.x)
  {
    printf("A copy\n");
    fflush(stdout);
  }

  int x;
};

static_assert(!std::is_trivial<A>::value, "");

struct HashA {
  zda_inline size_t operator()(A const &a) { return std::hash<int>()(a.x); }
};

zda_inline bool operator==(A const &a, A const &b) noexcept { return a.x == b.x; }

struct int_entry_t {
  A key;
  ZDA_HT_HOOK;
};

struct int_entry_get_key {
  zda_inline A const &operator()(int_entry_t const *entry) const noexcept { return entry->key; }
};

using TestHt = Ht<int_entry_t, A, int_entry_get_key, HashA>;

static void prepare_entries(TestHt &ht)
{
  for (int i = 0; i < 100; ++i) {
    auto entry = (int_entry_t *)malloc(sizeof(int_entry_t));
    entry->key = i;
    auto p_dup = ht.insert_entry(entry);
    ASSERT_TRUE(!p_dup);
  }
}

static void int_entry_print(zda_ht_node_t *node) noexcept
{
  auto entry = zda_ht_entry(node, int_entry_t);
  printf("%d", entry->key.x);
}

static_assert(std::is_same<TestHt::AKey, A const &>::value, "");

TEST(ht_test, insert)
{
  TestHt ht;
  prepare_entries(ht);
  // zda_ht_print_layout(&ht.rep(), int_entry_print);
  // for (auto const &ent : ht) {
  //   printf("%d ", ent.key.x);
  // }

  A    a(1);
  auto entry = ht.search(a);
  EXPECT_EQ(entry->key, 1);
}