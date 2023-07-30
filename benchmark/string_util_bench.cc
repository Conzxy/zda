#include "zda/string_util.hpp"

#include <benchmark/benchmark.h>

using namespace zda;
using namespace std;
using namespace benchmark;

const string s1 = "adfasdf";
const string s2 = "sdfqwerasdf";
const string s3 = "afklhqohjorhoah";

static void string_add_bench(State &state)
{
    for (auto _ : state) {
        std::string str;
        str += s1;
        str += s2;
        str += s3;
    }
}

static void string_util_bench(State &state)
{
    for (auto _ : state) {
        std::string str = str_catf("%S%S%S", &s1, &s2, &s3);
    }
}

#define register_benchmark(func, name)                                         \
    BENCHMARK(func)->RangeMultiplier(10)->Range(10, 1000000)->Name(name)

register_benchmark(string_add_bench, "std::string operator+=");
register_benchmark(string_util_bench, "str_catf");
