#include <benchmark/benchmark.h>

#include <ratio>
#include <set>

#include "zda/rb_tree.h"

#define USE_INPLACE 0

using namespace benchmark;

typedef struct int_entry {
  int           key;
  zda_rb_node_t node;
} int_entry_t;

static zda_inline int int_cmp(int x, int y) { return x - y; }

zda_def_rb_tree_insert_check(int_rb_tree_insert_check, int, int_entry_t, key, int_cmp)
zda_def_rb_tree_search(int_entry_search, int, int_entry, key, int_cmp)
zda_def_rb_tree_remove(int_entry_remove, int, int_entry, key, int_cmp)

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

int print_entry(void const *node)
{
  int_entry_t *entry = zda_rb_entry((zda_rb_node_t *)node, int_entry);
  return printf("%d", entry->key);
}

static void zda_rb_tree_insert_bench(State &state)
{
  const int       num = state.range(0);
  zda_rb_header_t header;
  zda_rb_header_init(&header);
  for (auto _ : state) {
    state.PauseTiming();
    int_entry          *p_dup;
    zda_rb_commit_ctx_t cmt_ctx;
    int                 result;
    state.ResumeTiming();

    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      zda_rb_tree_insert_inplace(&header, i, int_entry_t, key, int_cmp, result, p_dup);
      p_dup->key = i;
#else
      p_dup     = int_rb_tree_insert_check(&header, i, &cmt_ctx);
      auto node = (int_entry_t *)malloc(sizeof(int_entry_t));
      node->key = i;
      zda_rb_tree_insert_commit(&header, &cmt_ctx, &node->node);
#endif
      zda_rb_tree_print_tree(&header, print_entry);
      fflush(stdout);
    }

    state.PauseTiming();
    zda_rb_tree_destroy_init(&header, int_entry_free);
    state.ResumeTiming();
  }
}

static void stl_set_insert_bench(State &state)
{
  const int     num = state.range(0);
  std::set<int> tree;
  for (auto _ : state) {
    state.PauseTiming();
    std::pair<decltype(tree)::iterator, bool> iter_res;
    state.ResumeTiming();

    for (int i = 0; i < num; ++i) {
      iter_res = tree.insert(i);
    }
    state.PauseTiming();
    tree.clear();
    state.ResumeTiming();
  }
}

static void prepare_rb_tree(zda_rb_header *tree, int n)
{
  int_entry *p_dup;
  int        result;
  for (int i = 0; i < n; ++i) {
    zda_rb_tree_insert_inplace(tree, i, int_entry_t, key, int_cmp, result, p_dup);
    p_dup->key = i;
  }
}

static void prepare_stl_set(std::set<int> &set, int n)
{
  for (int i = 0; i < n; ++i) {
    set.insert(i);
  }
}

static void zda_rb_tree_search_bench(State &state)
{
  zda_rb_header_t header;
  zda_rb_header_init(&header);
  const int num = state.range(0);
  prepare_rb_tree(&header, num);
  int_entry *p_dup;

  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      zda_rb_tree_search_inplace(&header, i, int_entry, key, int_cmp, p_dup);
#else
      auto p_dup = int_entry_search(&header, i);
#endif
      if (p_dup->key != i) {
        abort();
      }
    }
  }
  zda_rb_tree_destroy(&header, int_entry_free);
}

static void stl_set_search_bench(State &state)
{
  std::set<int> tree;
  const int     num = state.range(0);
  prepare_stl_set(tree, num);

  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
      auto x = tree.find(i);
      if (*x != i) abort();
    }
  }
}

static void zda_rb_tree_remove_bench(State &state)
{
  const int num = state.range(0);

  zda_rb_header_t header;
  zda_rb_header_init(&header);
  for (auto _ : state) {
    state.PauseTiming();
    int_entry *p_entry;
    prepare_rb_tree(&header, num);
    state.ResumeTiming();

    for (int i = 0; i < num; ++i) {
#if USE_INPLACE
      zda_rb_tree_remove_inplace(&header, i, int_entry, key, int_cmp, p_entry);
#else
      p_entry    = int_entry_remove(&header, i);
#endif
      if (!p_entry) {
        printf("i = %d\n", i);
        abort();
      }
    }
    state.PauseTiming();
    zda_rb_tree_destroy_init(&header, int_entry_free);
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
      if (n == 0) abort();
    }

    state.PauseTiming();
    tree.clear();
    state.ResumeTiming();
  }
}

#define register_tree_benchmark(func, name)                                                        \
  BENCHMARK(func)->RangeMultiplier(10)->Range(10, 1000000)->Name(name)

register_tree_benchmark(zda_rb_tree_insert_bench, "zda_rb_tree insert");
register_tree_benchmark(stl_set_insert_bench, "std::set insert");
register_tree_benchmark(zda_rb_tree_search_bench, "zda_rb_tree search");
register_tree_benchmark(stl_set_search_bench, "std::set search");
register_tree_benchmark(zda_rb_tree_remove_bench, "zda_rb_tree remove");
register_tree_benchmark(stl_set_remove_bench, "std::set remove");
