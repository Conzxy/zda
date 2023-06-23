#include "zda/list.h"

#include <gtest/gtest.h>

typedef struct int_node {
  int             val;
  zda_list_node_t node;
} int_node_t;

int_node_t *new_int_node() { return (int_node_t *)malloc(sizeof(int_node_t)); }

static void prepare_list(zda_list_header_t *list, int n)
{
  for (int i = 0; i < n; ++i) {
    auto node = new_int_node();
    zda_list_push_back(list, &node->node);
    node->val = i;
  }
}

TEST(list_test, insert)
{
  constexpr int     n = 10;
  zda_list_header_t header;
  zda_list_header_init(&header);
  prepare_list(&header, n);
  zda_list_iterate(&header)
  {
    int_node_t *int_node = zda_list_entry(pos, int_node_t);
    printf("val = %d\n", int_node->val);
  }

  zda_list_destroy(&header, int_node_t, free);
}

static zda_inline zda_bool int_equal(int x, int y) { return x == y; }

TEST(list_test, search)
{
  zda_list_header_t header;
  zda_list_header_init(&header);
  prepare_list(&header, 100);

  int_node_t *p_entry;
  zda_list_search(&header, 40, int_node_t, val, int_equal, p_entry);

  ASSERT_TRUE(p_entry);
  EXPECT_TRUE(p_entry->val == 40);
  zda_list_destroy(&header, int_node_t, free);
}

TEST(list_test, remove)
{
  zda_list_header_t header;
  zda_list_header_init(&header);
  prepare_list(&header, 100);

  int i = 0;
  while (!zda_list_is_empty(&header)) {
    auto front = zda_list_front(&header);
    auto ent   = zda_list_entry(front, int_node_t);
    EXPECT_EQ(ent->val, i);
    ++i;
    zda_list_pop_front(&header);
    free(ent);
  }
}

static void prepare_list2(zda_list_header2_t *header, int n)
{
  for (int i = 0; i < n; ++i) {
    auto node = new_int_node();
    zda_list_push_back2(header, &node->node);
    node->val = i;
  }
}

TEST(list_test2, insert)
{
  zda_list_header2_t header;
  zda_list_header2_init(&header);
  prepare_list2(&header, 100);
  zda_list_iter2 iter = zda_list_first(&header);

  int i = 0;
  for (; !zda_list_is_terminator2(&header, &iter); zda_list_iter_inc2(&iter)) {
    auto entry = zda_list_entry(iter.node, int_node_t);
    printf("val = %d\n", entry->val);
    EXPECT_EQ(entry->val, i);
    ++i;
  }

  zda_list_destroy2(&header, int_node_t, free);
}

TEST(list_test2, search)
{
  zda_list_header2_t header;
  zda_list_header2_init(&header);
  prepare_list2(&header, 100);

  for (int i = 0; i < 100; ++i) {
    int_node_t *p_entry;
    zda_list_search2(&header, i, int_node_t, val, int_equal, p_entry);
    ASSERT_TRUE(p_entry);
    EXPECT_EQ(p_entry->val, i);
  }
  zda_list_destroy2(&header, int_node_t, free);
}