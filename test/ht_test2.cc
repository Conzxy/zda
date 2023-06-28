#include "zda/ht.h"
#include <zda/ht.hpp>

#include <gtest/gtest.h>

using namespace zda;

struct int_entry_t {
  int key;
  ZDA_HT_HOOK;
};

struct int_entry_get_key {
  zda_inline int operator()(int_entry_t const *entry) const noexcept { return entry->key; }
};

using TestHt = Ht<int_entry_t, int, int_entry_get_key>;

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
  printf("%d", entry->key);
}

TEST(ht_test, insert)
{
  TestHt ht;
  prepare_entries(ht);
  zda_ht_print_layout(&ht.rep(), int_entry_print);
  for (auto ent : ht) {
    printf("%d ", ent.key);
  }
}