#include "zda/list.h"

#include <gtest/gtest.h>

typedef struct int_node {
  int             val;
  zda_list_node_t node;
} int_node_t;

#define prepare_list(n)                                                                            \
  zda_list_header_t header;                                                                        \
  zda_list_node_header_init(&header);                                                              \
  int_node_t nodes[n];                                                                             \
  for (int i = 0; i < n; ++i) {                                                                    \
    zda_list_push_front(&header, &nodes[i].node);                                                  \
    nodes[i].val = i;                                                                              \
  }

TEST(list_test, insert)
{
  constexpr int n = 10;
  prepare_list(n)

  zda_list_iterate(&header)
  {
    int_node_t *int_node = zda_list_get_entry(int_node_t);
    printf("val = %d\n", int_node->val);
  }
}

TEST(list_test, search)
{
  prepare_list(100)

  int_node_t *p_entry = NULL;
  zda_list_search(&header, int_node_t, val, 40, p_entry);

  EXPECT_TRUE(p_entry->val == 40);
}
