find_package(benchmark REQUIRED)

function (add_benchmark)
  cmake_parse_arguments(PARSE_ARGV 0 CONZXY "" "TARGET" "SOURCES")
  if (ZDA_BUILD_TEST)
    message(STATUS "Build benchmark target: ${CONZXY_TARGET}")
    add_executable(${CONZXY_TARGET} ${CONZXY_SOURCES})
  else ()
    add_executable(${CONZXY_TARGET} EXCLUDE_FROM_ALL ${CONZXY_SOURCES})
  endif ()
  target_link_libraries(${CONZXY_TARGET} PRIVATE benchmark::benchmark benchmark::benchmark_main zda)
endfunction ()

file(GLOB_RECURSE ZDA_BENCHMARK_SOURCES "*bench*.cc")

foreach (zda_bench_src ${ZDA_BENCHMARK_SOURCES})
  get_filename_component(zda_bench_name ${zda_bench_src} NAME_WE)
  add_benchmark(TARGET ${zda_bench_name} SOURCES ${zda_bench_src})
endforeach ()
