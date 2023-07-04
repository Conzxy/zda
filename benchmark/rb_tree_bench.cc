#include <benchmark/benchmark.h>

#include <ratio>
#include <set>

#include "zda/rb_tree.h"
#include "zda/avl_tree.h"

#define USE_INPLACE 1

using namespace benchmark;

typedef struct int_entry {
  int           key;
  zda_rb_node_t node;
} int_entry_t;

typedef struct int_entry2 {
  int            key;
  zda_avl_node_t node;
} int_entry2_t;

static zda_inline int int_cmp(int x, int y) noexcept { return x - y; }

static zda_inline int int_entry_get_key(int_entry_t *p_entry) noexcept { return p_entry->key; }

static zda_inline int int_entry2_get_key(int_entry2_t *p_entry) noexcept { return p_entry->key; }

int print_entry(void const *node)
{
  int_entry_t *entry = zda_rb_entry((zda_rb_node_t *)node, int_entry);
  return printf("%d", entry->key);
}

int print_entry2(zda_avl_node_t const *node)
{
  int_entry2_t *entry = zda_avl_entry((zda_avl_node_t *)node, int_entry2);
  return printf("%d", entry->key);
}

zda_def_rb_tree_insert_check(int_rb_tree_insert_check, int, int_entry_t, int_entry_get_key, int_cmp)
zda_def_rb_tree_search(int_entry_search, int, int_entry, int_entry_get_key, int_cmp)
zda_def_rb_tree_remove(int_entry_remove, int, int_entry, int_entry_get_key, int_cmp)

zda_def_avl_tree_insert_check(
    int_avl_tree_insert_check,
    int,
    int_entry2_t,
    int_entry2_get_key,
    int_cmp
)
static zda_inline zda_def_avl_tree_destroy(int_avl_tree_destroy, int_entry2_t, free)
zda_def_avl_tree_search(int_avl_tree_search, int, int_entry2_t, int_entry2_get_key, int_cmp)
zda_def_avl_tree_remove(int_avl_tree_remove, int, int_entry2_t, int_entry2_get_key, int_cmp)

int int_entry_cmp(zda_rb_node_t const *node, void const *key)
{
  int_entry const *p_entry = zda_rb_entry(node, int_entry const);
  return p_entry->key - *(int *)key;
}

void int_entry_free(zda_rb_node_t *node)
{
  int_entry *p_entry = zda_rb_entry(node, int_entry);
  free(p_entry);
}

static void prepare_rb_tree(zda_rb_header *tree, int n)
{
  int_entry_t *p_dup;
  int_entry_t *entry;
  for (int i = 0; i < n; ++i) {
    entry      = (int_entry_t *)malloc(sizeof(int_entry_t));
    entry->key = i;
    zda_rb_tree_insert_entry_inplace(tree, entry, int_entry_t, int_entry_get_key, int_cmp, p_dup);
  }
}
static void prepare_avl_tree(zda_avl_tree_t *tree, int n)
{
  int_entry2_t *p_dup;
  int_entry2_t *entry;
  for (int i = 0; i < n; ++i) {
    entry      = (int_entry2_t *)malloc(sizeof(int_entry2_t));
    entry->key = i;
    zda_avl_tree_insert_entry_inplace(
        tree,
        entry,
        int_entry2_t,
        int_entry2_get_key,
        int_cmp,
        p_dup
    );
  }
}

static void prepare_stl_set(std::set<int> &set, int n)
{
  for (int i = 0; i < n; ++i) {
    set.insert(i);
  }
}

static void zda_rb_tree_insert_bench(State &state)
{
  const int       num = state.range(0);
  zda_rb_header_t header;
  zda_rb_header_init(&header);

  zda_rb_commit_ctx_t cmt_ctx;
  int_entry_t        *p_dup;
  int_entry_t        *entry;

  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      entry      = (int_entry_t *)malloc(sizeof(int_entry_t));
      entry->key = i;
      zda_rb_tree_insert_entry_inplace(
          &header,
          entry,
          int_entry_t,
          int_entry_get_key,
          int_cmp,
          p_dup
      );
#else
      p_dup     = int_rb_tree_insert_check(&header, i, &cmt_ctx);
      auto node = (int_entry_t *)malloc(sizeof(int_entry_t));
      node->key = i;
      zda_rb_tree_insert_commit(&header, &cmt_ctx, &node->node);
#endif
    }

    state.PauseTiming();
    zda_rb_tree_destroy_init(&header, int_entry_free);
    state.ResumeTiming();
  }
}

static void zda_avl_tree_insert_bench(State &state)
{
  const int      num = state.range(0);
  zda_avl_tree_t header;
  zda_avl_tree_init(&header);
  int_entry2_t        *p_dup;
  zda_avl_commit_ctx_t cmt_ctx;
  int                  result;

  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      auto entry = (int_entry2_t *)malloc(sizeof(int_entry2_t));
      entry->key = i;
      zda_avl_tree_insert_entry_inplace(
          &header,
          entry,
          int_entry2_t,
          int_entry2_get_key,
          int_cmp,
          p_dup
      );
#else
      p_dup     = int_avl_tree_insert_check(&header, i, &cmt_ctx);
      auto node = (int_entry2_t *)malloc(sizeof(int_entry_t));
      node->key = i;
      zda_avl_tree_insert_commit(&header, &cmt_ctx, &node->node);
#endif
    }

    state.PauseTiming();
    int_avl_tree_destroy(&header);
    zda_avl_tree_init(&header);
    state.ResumeTiming();
  }
}

static void stl_set_insert_bench(State &state)
{
  const int                                 num = state.range(0);
  std::set<int>                             tree;
  std::pair<decltype(tree)::iterator, bool> iter_res;
  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
      iter_res = tree.insert(i);
    }
    state.PauseTiming();
    tree.clear();
    state.ResumeTiming();
  }
}

static void zda_rb_tree_search_bench(State &state)
{
  const int num = state.range(0);

  zda_rb_header_t header;
  zda_rb_header_init(&header);
  prepare_rb_tree(&header, num);

  int_entry *p_dup;
  // printf("rb height = %zu\n", zda_rb_tree_get_height(&header, zda_rb_tree_get_root(&header)));

  bool has_insert = false;
  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      zda_rb_tree_search_inplace(&header, i, int_entry, int_entry_get_key, int_cmp, p_dup);
#else
      p_dup   = int_entry_search(&header, i);
#endif
      state.PauseTiming();
      if (p_dup->key != i) {
        abort();
      }
      state.ResumeTiming();
    }
  }
  zda_rb_tree_destroy_init(&header, int_entry_free);
}

static void zda_avl_tree_search_bench(State &state)
{
  const int num = state.range(0);

  zda_avl_tree_t header;
  zda_avl_tree_init(&header);
  prepare_avl_tree(&header, num);

  int_entry2_t *p_dup;

  // printf("avl height = %zu\n", zda_avl_tree_get_height(&header));
  // zda_avl_tree_print_tree(&header, print_entry2);
  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      zda_avl_tree_search_inplace(&header, i, int_entry2_t, int_entry2_get_key, int_cmp, p_dup);
#else
      p_dup   = int_avl_tree_search(&header, i);
#endif
      state.PauseTiming();
      if (p_dup->key != i) {
        abort();
      }
      state.ResumeTiming();
    }
  }
  int_avl_tree_destroy(&header);
}

static void stl_set_search_bench(State &state)
{
  std::set<int>            tree;
  const int                num = state.range(0);
  decltype(tree)::iterator iter;
  prepare_stl_set(tree, num);

  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
      iter = tree.find(i);
      state.PauseTiming();
      if (*iter != i) {
        abort();
      }
      state.ResumeTiming();
    }
  }
  tree.clear();
}

static void zda_rb_tree_remove_bench(State &state)
{
  const int num = state.range(0);

  zda_rb_header_t header;
  zda_rb_header_init(&header);
  int_entry_t *p_entry;
  for (auto _ : state) {
    state.PauseTiming();
    prepare_rb_tree(&header, num);
    state.ResumeTiming();

    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      zda_rb_tree_remove_inplace(&header, i, int_entry, int_entry_get_key, int_cmp, p_entry);
#else
      p_entry = int_entry_remove(&header, i);
#endif
      free(p_entry);
    }
    state.PauseTiming();
    zda_rb_tree_destroy_init(&header, int_entry_free);
    state.ResumeTiming();
  }
}

static void zda_avl_tree_remove_bench(State &state)
{
  const int num = state.range(0);

  zda_avl_tree_t header;
  zda_avl_tree_init(&header);
  int_entry2_t *p_entry;
  for (auto _ : state) {
    state.PauseTiming();
    prepare_avl_tree(&header, num);
    state.ResumeTiming();

    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      zda_avl_tree_remove_inplace(&header, i, int_entry2_t, int_entry2_get_key, int_cmp, p_entry);
#else
      p_entry = int_avl_tree_remove(&header, i);
#endif
      free(p_entry);
    }
    state.PauseTiming();
    int_avl_tree_destroy(&header);
    zda_avl_tree_init(&header);
    state.ResumeTiming();
  }
}

static void stl_set_remove_bench(State &state)
{
  std::set<int> tree;
  const int     num = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();
    prepare_stl_set(tree, num);
    size_t n;
    state.ResumeTiming();

    for (int i = 0; i < num; ++i) {
      n = tree.erase(i);
      // if (n == 0) abort();
    }

    state.PauseTiming();
    tree.clear();
    state.ResumeTiming();
  }
}

#define register_tree_benchmark(func, name)                                                        \
  BENCHMARK(func)->RangeMultiplier(10)->Range(10, 1000000)->Name(name)

register_tree_benchmark(zda_rb_tree_search_bench, "zda_rb_tree search");
register_tree_benchmark(zda_avl_tree_search_bench, "zda_avl_tree search");
register_tree_benchmark(stl_set_search_bench, "std::set search");
register_tree_benchmark(zda_avl_tree_insert_bench, "zda_avl_tree insert");
register_tree_benchmark(zda_rb_tree_insert_bench, "zda_rb_tree insert");
register_tree_benchmark(stl_set_insert_bench, "std::set insert");
register_tree_benchmark(zda_avl_tree_remove_bench, "zda_avl_tree remove");
register_tree_benchmark(zda_rb_tree_remove_bench, "zda_rb_tree remove");
register_tree_benchmark(stl_set_remove_bench, "std::set remove");
