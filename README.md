# Zda
## Introduction
The repo is a toolbox that contains various pratical data structures and algorithms, such as red-black tree, linked-list, etc.  
These data structures can be split into two catogories:
* Intrusive data structures
* Non-intrusive data structure  

These intrusive data structures are written in `C` programming language. I think use `C` instead of `C++` is more great choice since I think the `template` is also evil in some scenarios, eg. It requires you write all code in header to generic data structure. To instrusive design, `template` is not required to record the compile time information, such as typename, etc. Thus, I use `C` to complete them.  
The intrusive design to make the data structure be generic, flexible and powerful, eg. you can insert nodes but don't cause dynamic memory allocation, that is, these nodes can be allocated in stack or code segments(global variable). Besides, you can allocate multiple nodes in the same memory region to get great memory locality.

To some data structures, I use `C++` with tempaltes, e.g., generic dynamic array, it is difficult to `C`. If you use macros to implement it, user must register entry type to function-like macro every time. Other methods are wrong, since `C` lacks of template.
## Schedule
* [x] [Intrusive Red-black tree](zda/rb_tree.h)  
相关文档参考[rb_tree.h](zda/rb_tree.h)  
使用方式参考[单元测试文件](test/rb_tree_test.cc)和[基准测试文件](benchmark/rb_tree_bench.cc)  
* [x] [Intrusive double linked-list](zda/list.h)  
  * [x] Sentinel version  
  * [x] No sentinel version  
相关文档参考[list.h](zda/list.h)  
使用方式参考[单元测试文件](test/ht_test.cc)  
* [x] [Intrusive single linked-list](zda/slist.h)  
  * [x] Sentinel version  
  * [x] No sentinel version  
相关文档参考[slist.h](zda/slist.h)  
使用方式参考[单元测试文件](test/slist_test.cc)  
* [x] [Half-Intrusive hash table(Based on single-linked-list)](zda/ht.h)  
相关文档参考[ht.h](zda/ht.h)  
使用方式参考[单元测试文件](test/ht_test.cc)  
* [x] [Reserved array](zda/reserved_array.hpp)  
`Reserved array`是一个预分配的数组，用户在使用前必须先分配空间，然后访问元素。  
同时，这个数据结构也是`Dynamic array`的底层实现。  
相关文档参考[reserved_array.h](zda/reserved_array.hpp)  
使用风格类似STL。
* [x] [Dynamic array](zda/darray.hpp)  
相关文档参考[darray.hpp](zda/darray.hpp)  
使用风格类似STL，亦可参考[单元测试文件](test/darray_test2.cc)  
* [ ] Half-Intrusive hash table(Based on Red-black tree)
* [ ] Intrusive avl tree

## Benchmark
TODO...