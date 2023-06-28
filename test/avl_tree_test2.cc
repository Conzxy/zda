#include "zda/avl_tree.h"
#include <zda/avl_tree.hpp>

#include <gtest/gtest.h>

using namespace zda;

typedef struct int_entry {
  int key;
  ZDA_AVL_NODE_HOOK;
} int_entry_t;

struct get_key_int_entry {
  zda_inline int operator()(int_entry_t const *entry) const noexcept { return entry->key; }
};

struct compare_int_entry {
  zda_inline int operator()(int x, int y) const noexcept { return x - y; };
};

struct free_int_entry {
  zda_inline void operator()(int_entry_t *entry) const noexcept { free(entry); }
};

static int print_int_entry(zda_avl_node_t const *node)
{
  auto entry = zda_avl_entry(node, int_entry_t const);
  return printf("%d", entry->key);
}

TEST(avl_test, insert)
{
  AvlTree<int, int_entry_t, get_key_int_entry> tree;

  for (int i = 0; i < 100; ++i) {
    int_entry_t *entry = (int_entry_t *)malloc(sizeof(int_entry_t));
    entry->key         = i;
    // tree.insert_entry(entry);
    zda_avl_commit_ctx_t ctx;
    auto                 p_dup = tree.insert_check(i, &ctx);
    ASSERT_TRUE(!p_dup);

    tree.insert_commit(&ctx, &entry->node);
  }

  auto beg = tree.begin();
  // zda_avl_tree_print_tree(&tree.rep(), print_int_entry);
  //
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(beg->key, i);
    ++beg;
  }
}
