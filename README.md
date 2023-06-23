# Zda
## Introduction
The repo is a toolbox that contains various pratical data structures and algorithms, such as red-black tree, linked-list, etc.  
These are written in `C` programming language. I think use `C` instead of `C++` is more great choice since I think the `template` is also evil in some scenarios, eg. It requires you write all code in header to generic data structure.  
I use `void*` and intrusive design to make the data structure be generic, more flexible and powerful, eg. you can insert nodes but don't cause dynamic memory allocation, that is, these nodes can be allocated in stack or code segments(global variable).

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
* [ ] Half-Intrusive hash table(Based on Red-black tree)
* [ ] Intrusive avl tree

## Benchmark
TODO...