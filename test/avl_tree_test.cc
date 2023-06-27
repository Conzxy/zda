#include <zda/avl_tree.h>

#include <gtest/gtest.h>

typedef struct int_entry {
  zda_avl_node_t node;
  int            key;
} int_entry_t;

static int print_int_entry(zda_avl_node_t const *node)
{
  auto entry = zda_avl_entry(node, int_entry_t const);
  return printf("%d", entry->key);
}

static zda_inline int int_entry_cmp(int x, int y) { return x - y; }
zda_def_avl_tree_insert_check(avl_tree_insert_check_int_entry, int, int_entry_t, key, int_entry_cmp)
zda_def_avl_tree_destroy(avl_tree_destroy_int_entry, int_entry_t, free)
zda_def_avl_tree_search(avl_tree_search_int_entry, int, int_entry_t, key, int_entry_cmp)
zda_def_avl_tree_remove(avl_tree_remove_int_entry, int, int_entry, key, int_entry_cmp)

TEST(avl_tree_test, insert)
{
  zda_avl_tree_t tree;
  zda_avl_tree_init(&tree);

  int_entry            entries[100];
  zda_avl_commit_ctx_t commit_ctx;

  for (int i = 0; i < 100; ++i) {
    auto *p_dup = avl_tree_insert_check_int_entry(&tree, i, &commit_ctx);

    ASSERT_FALSE(p_dup);

    entries[i].key = i;
    zda_avl_node_init(&entries[i].node);
    zda_avl_tree_insert_commit(&tree, &commit_ctx, &entries[i].node);

    p_dup = avl_tree_insert_check_int_entry(&tree, i, &commit_ctx);
    ASSERT_TRUE(p_dup);
    EXPECT_EQ(p_dup->key, i);
  }

  ASSERT_TRUE(zda_avl_tree_verify_properties(&tree));

  auto first = zda_avl_tree_get_first(&tree);
  int  i     = 0;
  for (; first != zda_avl_tree_get_terminator(&tree); first = zda_avl_node_get_next(first)) {
    auto entry = zda_avl_entry(first, int_entry_t);
    EXPECT_EQ(entry->key, i);
    ++i;
  }
}

zda_inline static int_entry_t *new_int_entry(int key)
{
  auto ret = (int_entry_t *)malloc(sizeof(int_entry_t));
  ret->key = key;
  zda_avl_node_init(&ret->node);
  return ret;
}

static void prepare_avl_tree(zda_avl_tree_t *tree, int n)
{
  zda_avl_tree_init(tree);
  zda_avl_commit_ctx_t commit_ctx;
  for (int i = 0; i < n; ++i) {
    auto p_dup = avl_tree_insert_check_int_entry(tree, i, &commit_ctx);

    auto node = &new_int_entry(i)->node;
    zda_avl_tree_insert_commit(tree, &commit_ctx, node);
  }
}

TEST(avl_tree_test, search)
{
  zda_avl_tree_t tree;
  prepare_avl_tree(&tree, 100);
  // zda_avl_tree_print_tree(&tree, print_int_entry);

  for (int i = 0; i < 100; ++i) {
    auto p_entry = avl_tree_search_int_entry(&tree, i);

    ASSERT_TRUE(p_entry) << "i = " << i;
    ASSERT_EQ(p_entry->key, i) << "i = " << i;
  }

  avl_tree_destroy_int_entry(&tree);
}

TEST(avl_tree_test, remove)
{
  zda_avl_tree_t tree;
  prepare_avl_tree(&tree, 100);

  for (int i = 0; i < 100; ++i) {
    // zda_avl_tree_print_tree(&tree, print_int_entry);
    auto entry = avl_tree_remove_int_entry(&tree, i);
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->key, i);
    free(entry);
  }
}