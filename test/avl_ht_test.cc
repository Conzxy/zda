#include <zda/avl_ht.h>
#include <stdio.h>

#include <gtest/gtest.h>

typedef struct int_entry {
  int               key;
  zda_avl_ht_node_t node;
} int_entry_t;

static zda_inline size_t int_entry_hash(int i) noexcept { return i ^ ~i; }

static zda_inline zda_bool int_entry_equal(int i, int j) noexcept { return i == j; }

static zda_inline int int_entry_cmp(int i, int j) noexcept { return i - j; }
static void           int_entry_print(zda_avl_ht_node_t *node) noexcept
{
  auto entry = zda_avl_ht_entry(node, int_entry);
  printf("%d", entry->key);
}

static zda_inline int int_entry_get_key(int_entry_t *entry) noexcept { return entry->key; }

zda_def_avl_ht_destroy(ht_destroy_int_entry, int_entry_t, free)

zda_def_avl_ht_insert_check(
    ht_insert_check_int_entry,
    int,
    int_entry_t,
    int_entry_get_key,
    int_entry_hash,
    int_entry_cmp
)

zda_def_avl_ht_search(
    ht_search_int_entry,
    int,
    int_entry_t,
    int_entry_get_key,
    int_entry_hash,
    int_entry_cmp
)

zda_def_avl_ht_remove(
    ht_remove_int_entry,
    int,
    int_entry_t,
    int_entry_get_key,
    int_entry_hash,
    int_entry_cmp
)

zda_def_avl_ht_insert_entry(
    ht_insert_int_entry,
    int_entry_t,
    int_entry_get_key,
    int_entry_hash,
    int_entry_cmp
)

static void prepare_ht(zda_avl_ht_t *ht, size_t n)
{
  for (int i = 0; i < n; ++i) {
    zda_avl_ht_commit_ctx_t commit_ctx;
    int_entry              *p_dup;
    zda_avl_ht_insert_check_inplace(
        ht,
        i,
        int_entry_t,
        int_entry_get_key,
        int_entry_hash,
        int_entry_cmp,
        commit_ctx,
        p_dup
    );
    ASSERT_TRUE(!p_dup);
    int_entry_t *entry = (int_entry_t *)malloc(sizeof(int_entry_t));
    entry->key         = i;
    zda_avl_ht_insert_commit(ht, &commit_ctx, &(entry->node));
    zda_avl_ht_print_layout(ht, int_entry_print);
  }
}

TEST(ht_test, insert)
{
  zda_avl_ht_t ht;
  zda_avl_ht_init(&ht);
  prepare_ht(&ht, 10);

  auto first = zda_avl_ht_get_first(&ht);
  for (; !zda_avl_ht_iter_is_terminator(&first); zda_avl_ht_iter_inc(&first)) {
    auto entry = zda_avl_ht_iter2entry(first, int_entry_t);
  }

  zda_avl_ht_destroy_inplace(&ht, int_entry_t, free);
}

TEST(ht_test, search)
{
  zda_avl_ht_t ht;
  zda_avl_ht_init(&ht);
  prepare_ht(&ht, 100);

  for (int i = 0; i < 100; ++i) {
    auto entry = ht_search_int_entry(&ht, i);
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->key, i);
  }
  zda_avl_ht_destroy_inplace(&ht, int_entry_t, free);
}

TEST(ht_test, remove)
{
  zda_avl_ht_t ht;
  zda_avl_ht_init(&ht);
  prepare_ht(&ht, 100);

  for (int i = 0; i < 100; ++i) {
    int_entry_t *result;
    ASSERT_TRUE(ht_search_int_entry(&ht, i));
#if 1
    zda_avl_ht_remove_inplace(
        &ht,
        i,
        int_entry_t,
        int_entry_get_key,
        int_entry_hash,
        int_entry_cmp,
        result
    );
#else
    result = ht_remove_int_entry(&ht, i);
#endif
    ASSERT_TRUE(result) << i;
    free(result);
  }
  zda_avl_ht_destroy_inplace(&ht, int_entry_t, free);
}