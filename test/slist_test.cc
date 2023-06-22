#include <gtest/gtest.h>

#include <zda/slist.h>

typedef struct int_entry {
  int              val;
  zda_slist_node_t node;
} int_entry_t;

static zda_inline int_entry_t *new_int_entry(int v)
{
  int_entry_t *entry = (int_entry_t *)malloc(sizeof(int_entry));
  if (!entry) abort();
  entry->val = v;
  return entry;
}

#define get_int_entry(node) zda_slist_entry(node, int_entry)

TEST(slist_test, insert)
{
  zda_slist_header_t list;
  zda_slist_header_init(&list);

  for (int i = 0; i < 10; ++i) {
    zda_slist_push_front(&list, &new_int_entry(i)->node);
  }

  int i = 0;
  zda_slist_iterate(&list)
  {
    printf(" i = %d\n", i);
    auto x = 9 - i;
    EXPECT_EQ(get_int_entry(pos)->val, x) << x;
    ++i;
  }

  zda_slist_destroy(&list, int_entry_t);
}

static void prepare_slist(zda_slist_header_t *header, int n)
{
  for (int i = 0; i < n; ++i) {
    zda_slist_push_front(header, &new_int_entry(i)->node);
  }
}

int int_entry_cmp(zda_slist_node_t const *node, void const *key)
{
  return zda_slist_entry(node, int_entry_t const)->val == *(int const *)key;
}

TEST(slist_test, search)
{
  zda_slist_header_t header;
  zda_slist_header_init(&header);
  prepare_slist(&header, 10);

  for (int i = 0; i < 10; ++i) {
    auto node = zda_slist_search(&header, &i, int_entry_cmp);
    ASSERT_TRUE(node);
    EXPECT_EQ(get_int_entry(node)->val, i);
  }
  zda_slist_destroy(&header, int_entry_t);
}

TEST(slist_test, remove)
{
  zda_slist_header_t header;
  zda_slist_header_init(&header);

  prepare_slist(&header, 10);

  for (int i = 0; i < 10; ++i) {
    auto front_node = zda_slist_front(&header);
    auto entry      = get_int_entry(front_node);
    ASSERT_TRUE(front_node);
    EXPECT_EQ(entry->val, 9 - i);
    free(entry);
    zda_slist_pop_front(&header);
  }

  zda_slist_destroy(&header, int_entry_t);
}

static void prepare_slist2(zda_slist2_t *slist, int n)
{
  for (int i = 0; i < n; ++i) {
    zda_slist_push_front2(slist, &new_int_entry(i)->node);
  }
}

TEST(slist2_test, insert)
{
  zda_slist2_t slist;
  zda_slist_header_init2(&slist);
  prepare_slist2(&slist, 100);

  int i = 99;
  zda_slist_iterate2(&slist)
  {
    auto entry = zda_slist_entry(pos, int_entry);
    EXPECT_EQ(entry->val, i);
    --i;
  }

  zda_slist_destroy2(&slist, int_entry_t);
}

TEST(slist2_test, pop)
{
  zda_slist_header2_t slist;
  zda_slist_header_init2(&slist);
  prepare_slist2(&slist, 100);

  int i = 99;

  while (!zda_slist_is_empty2(&slist)) {
    auto node  = zda_slist_front2(&slist);
    auto entry = zda_slist_entry(node, int_entry_t);
    EXPECT_EQ(i, entry->val);
    zda_slist_pop_front2(&slist);
    free(entry);
    --i;
  }

  zda_slist_destroy2(&slist, int_entry_t);
}

TEST(slist2_test, search)
{
  zda_slist_header2_t slist;
  zda_slist_header_init2(&slist);
  prepare_slist2(&slist, 100);

  for (int i = 0; i < 100; ++i) {
    auto node = zda_slist_search2(&slist, &i, int_entry_cmp);
    ASSERT_TRUE(node);
    auto entry = zda_slist_entry(node, int_entry_t);
    EXPECT_EQ(entry->val, i);
  }

  zda_slist_destroy2(&slist, int_entry_t);
}