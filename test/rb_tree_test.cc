#include "zda/rb_tree.h"

#include <gtest/gtest.h>

typedef struct int_entry {
  int           key;
  zda_rb_node_t node;
} int_entry_t;

int int_entry_cmp(int_entry_t const *entry, int key) { return entry->key - key; }

int int_entry_cmp2(zda_rb_node_t const *node, void const *key)
{
  int_entry_t const *entry = zda_rb_node_entry(node, int_entry_t const);
  int const         *_key  = (int const *)key;
  return entry->key - *_key;
}

int print_entry(void const *node)
{
  int_entry_t *entry = zda_rb_node_entry((zda_rb_node_t *)node, int_entry);
  return printf("%d", entry->key);
}

zda_def_rb_tree_insert(int_rb_tree_insert, int, int_entry_t)

static zda_inline void prepare_entries(zda_rb_header_t *header, int n)
{
  for (int i = 0; i < n; i++) {
    int_entry_t *p_dup  = ZDA_NULL;
    int          result = 0;
    result              = int_rb_tree_insert(header, &i, int_entry_cmp2, &p_dup);

    if (result) {
      p_dup->key = i;
    }
  }
}

TEST(rb_tree_test, insert)
{
  zda_rb_header_t header;
  zda_rb_header_init(&header);

  for (int i = 0; i < 100; i++) {
    int_entry_t *p_dup  = ZDA_NULL;
    int          result = 0;
    result              = int_rb_tree_insert(&header, &i, int_entry_cmp2, &p_dup);

    if (result) {
      p_dup->key = i;
      printf("insert ok: %d\n", p_dup->key);
      zda_rb_tree_print_tree(&header, print_entry);
      puts("");
    } else {
      printf("Failed to insert: %d\n", p_dup->key);
    }
  }

  auto min_entry = zda_rb_node_entry(zda_rb_tree_first(&header), int_entry_t);
  auto max_entry = zda_rb_node_entry(zda_rb_tree_last(&header), int_entry_t);

  printf("min_val = %d\n", min_entry->key);
  printf("max_val = %d\n", max_entry->key);

  zda_rb_tree_iterate(&header)
  {
    int_entry_t *entry = zda_rb_node_entry(pos, int_entry_t);
    printf("val: %d\n", entry->key);
  }

  zda_rb_tree_destroy_inplace(&header, int_entry_t, free);
}

TEST(rb_tree_test, search)
{
  zda_rb_header_t header;
  zda_rb_header_init(&header);
  const int n = 100;
  prepare_entries(&header, n);

  for (int i = 0; i < n; i++) {
    auto res = zda_rb_tree_search(&header, &i, int_entry_cmp2);
    ASSERT_TRUE(res);
    auto entry = zda_rb_node_entry(res, int_entry_t);
    EXPECT_EQ(entry->key, i);
  }
}

TEST(rb_tree_test, remove)
{
  zda_rb_header_t header;
  zda_rb_header_init(&header);
  const int n = 100;
  prepare_entries(&header, n);

  printf("The tree before remove: \n");
  zda_rb_tree_print_tree(&header, print_entry);

  for (int i = 0; i < n; ++i) {
    zda_rb_node_t *old_node = zda_rb_tree_remove(&header, &i, int_entry_cmp2);

    zda_rb_tree_print_tree(&header, print_entry);
    fflush(stdout);
    free(zda_rb_node_entry(old_node, int_entry_t));
  }

  printf("Remove complete: \n");
  zda_rb_tree_print_tree(&header, print_entry);
}