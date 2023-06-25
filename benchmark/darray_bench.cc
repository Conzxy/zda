#include <zda/darray.hpp>

#include <vector>
#include <benchmark/benchmark.h>

using namespace benchmark;

template <typename T>
static void bm_darray_add(State &state)
{
  auto num = state.range(0);

  zda::Darray<T> arr;
  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
      arr.Add(i);
    }
  }
}

template <typename T>
static void bm_vector_add(State &state)
{
  std::vector<T> arr;

  auto num = state.range(0);
  for (auto _ : state) {
    for (int i = 0; i < num; ++i) {
      arr.emplace_back(i);
    }
  }
}

template <typename T>
static void bm_darray_rm(State &state)
{

  auto num = state.range(0);
  for (auto _ : state) {
    state.PauseTiming();
    zda::Darray<T> arr;
    for (int i = 0; i < num; ++i) {
      arr.Add(i);
    }
    state.ResumeTiming();

    for (int i = 0; i < num; ++i) {
      arr.Remove();
    }
  }
}

template <typename T>
static void bm_vector_rm(State &state)
{

  auto num = state.range(0);
  for (auto _ : state) {
    state.PauseTiming();
    std::vector<T> arr;
    for (int i = 0; i < num; ++i) {
      arr.emplace_back(i);
    }
    state.ResumeTiming();

    for (int i = 0; i < num; ++i) {
      arr.pop_back();
    }
  }
}

#define DARRAY_BM(func, name) BENCHMARK(func)->Name(name)->RangeMultiplier(10)->Range(10, 1000000)

struct A {
  A(int x_) { x = x_; }

  int x;
};

DARRAY_BM(bm_darray_rm<A>, "Darray<A>::Remove()");
DARRAY_BM(bm_vector_rm<A>, "std::vector<A>::pop_back()");
DARRAY_BM(bm_darray_rm<int>, "Darray<int>::Remove()");
DARRAY_BM(bm_vector_rm<int>, "std::vector<int>::pop_back()");
DARRAY_BM(bm_darray_add<A>, "Darray<A>::Add()");
DARRAY_BM(bm_vector_add<A>, "std::vector<A>::emplace_back()");
DARRAY_BM(bm_darray_add<int>, "Darray<int>::Add()");
DARRAY_BM(bm_vector_add<int>, "std::vector<int>::emplace_back()");