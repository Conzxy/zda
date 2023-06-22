#include <zda/ht.h>

#include <gtest/gtest.h>

typedef struct int_entry {
  int           key;
  zda_ht_node_t node;
} int_entry_t;

static zda_inline size_t int_entry_hash(int i) { return i; }

static zda_inline zda_bool int_entry_equal(int i, int j) { return i == j; }

static void int_entry_print(zda_ht_node_t *node)
{
  auto entry = zda_ht_entry(node, int_entry);
  printf("%d", entry->key);
}

zda_def_ht_search(ht_search_int_entry, int, int_entry_t, key, int_entry_hash, int_entry_equal)

zda_def_ht_insert_check(
    ht_insert_check_int_entry,
    int,
    int_entry_t,
    key,
    int_entry_hash,
    int_entry_equal
)

zda_def_ht_insert_commit(ht_insert_commit_int_entry, int_entry_t)

zda_def_ht_remove(ht_remove_int_entry, int, int_entry_t, key, int_entry_hash, int_entry_equal)

static void prepare_ht(zda_ht_t *ht, size_t n)
{
  for (int i = 0; i < n; ++i) {
    zda_ht_commit_ctx_t commit_ctx;
    int                 result;
    int_entry          *p_dup;
    zda_ht_insert_check_inplace(
        ht,
        i,
        int_entry_t,
        key,
        int_entry_hash,
        int_entry_equal,
        commit_ctx,
        p_dup,
        result
    );
    ASSERT_TRUE(result);
    int_entry_t *entry = (int_entry_t *)malloc(sizeof(int_entry_t));
    entry->key         = i;
    zda_ht_insert_commit_inplace(ht, commit_ctx, &(entry->node));
  }
}

TEST(ht_test, insert)
{
  zda_ht_t ht;
  zda_ht_init(&ht);
  prepare_ht(&ht, 10);

  auto first = zda_ht_get_first(&ht);
  for (; !zda_ht_iter_is_terminator(&first); zda_ht_iter_inc(&first)) {
    auto entry = zda_ht_iter2entry(first, int_entry);
    printf("entry: %d\n", entry->key);
  }

  zda_ht_print_layout(&ht, int_entry_print);
  zda_ht_destroy_inplace(&ht, int_entry_t, free);
}

TEST(ht_test, search)
{
  zda_ht_t ht;
  zda_ht_init(&ht);
  prepare_ht(&ht, 100);

  for (int i = 0; i < 100; ++i) {
    auto entry = ht_search_int_entry(&ht, i);
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->key, i);
  }
  zda_ht_destroy_inplace(&ht, int_entry_t, free);
}

TEST(ht_test, remove)
{
  zda_ht_t ht;
  zda_ht_init(&ht);
  prepare_ht(&ht, 100);

  for (int i = 0; i < 100; ++i) {
    int_entry_t *result;
    zda_ht_remove_replace(&ht, i, int_entry_t, key, int_entry_hash, int_entry_equal, result);
    ASSERT_TRUE(result);
    free(result);
    zda_ht_print_layout(&ht, int_entry_print);
  }
  zda_ht_destroy_inplace(&ht, int_entry_t, free);
}