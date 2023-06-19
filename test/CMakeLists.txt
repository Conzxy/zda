find_package(GTest REQUIRED)

function (add_test target)
  if (ZDA_BUILD_TEST)
    message(STATUS "Build test target: ${target}")
    add_executable(${target} ${ARGN})
  else ()
    add_executable(${target} EXCLUDE_FROM_ALL ${ARGN})
  endif ()
  target_link_libraries(${target} GTest::gtest_main GTest::gtest zda)
endfunction ()

file(GLOB_RECURSE ZDA_TEST_SOURCES "*test*.cc")
message(STATUS "test sources: ${ZDA_TEST_SOURCES}")

foreach (zda_test_src ${ZDA_TEST_SOURCES})
  get_filename_component(zda_test_name ${zda_test_src} NAME_WE)
  add_test(${zda_test_name} ${zda_test_src})
endforeach ()
