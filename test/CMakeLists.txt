function (add_test target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} gtest_main gtest zda)
endfunction ()

add_test(list_test list_test.cc)
add_test(rb_tree_test rb_tree_test.cc)
