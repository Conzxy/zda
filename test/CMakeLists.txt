function (add_test target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} gtest_main gtest zda)
endfunction ()

add_test(list_test list_test.cc)
add_executable(list_test2 list_test.c)
target_link_libraries(list_test2 zda)