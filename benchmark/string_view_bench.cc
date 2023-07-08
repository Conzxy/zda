#include <zda/string_view.h>

#include <benchmark/benchmark.h>

using namespace benchmark;

zda_inline int std_case_compare(zda_string_view_t *view, zda_string_view str) zda_noexcept
{
    int r = strncasecmp(view->data, str.data,
                                   view->len < str.len ? view->len : str.len);

    if (r == 0) {
      if (view->len < str.len)
        r = -1;
      else if (view->len > str.len)
        r = 1;
    }

    return r;
}

static void zda_casecmp_bench(State &state)
{
    auto num = state.range(0);

    for (auto _ : state) {
        zda_string_view_t sstr;
        zda_string_view_literal_init(&sstr, "afdfd");
        zda_string_view_t lstr;
        zda_string_view_literal_init(&lstr, "asdjfljlkjald");
        zda_string_view_t mstr;
        zda_string_view_literal_init(&mstr, "adfadsfasd");

        int x  = zda_string_view_case_compare(&mstr, sstr);
        if (x) {
        }
        x = zda_string_view_case_compare(&mstr, lstr);
        if (x) {
        }
        x = zda_string_view_case_compare(&mstr, mstr);
        if (x) {
            abort();
        }
    }
}

static void std_casecmp_bench(State &state)
{
    auto num = state.range(0);

    for (auto _ : state) {
        zda_string_view_t sstr;
        zda_string_view_literal_init(&sstr, "afdfd");
        zda_string_view_t lstr;
        zda_string_view_literal_init(&lstr, "asdjfljlkjald");
        zda_string_view_t mstr;
        zda_string_view_literal_init(&mstr, "adfadsfasd");

        int x = std_case_compare(&mstr, sstr);
        if (x) {
        }
        x = std_case_compare(&mstr, lstr);
        if (x) {
        }
        x = std_case_compare(&mstr, mstr);
        if (x) {
            abort();
        }
    }
}

#define register_view_benchmark(func, name)                                                        \
  BENCHMARK(func)->RangeMultiplier(10)->Range(10, 1000000)->Name(name)

register_view_benchmark(std_casecmp_bench, "libc strncasecmp()");
register_view_benchmark(zda_casecmp_bench, "zda_string_view_case_compare()");
