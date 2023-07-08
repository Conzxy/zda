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
* [x] [Intrusive avl tree](zda/avl_tree.h)  
相关文档参考[avl_tree.h](zda/avl_tree.h)  
使用风格类似 `Red-Black Tree`，亦可参考[单元测试文件](test/darray_test2.cc)  
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
* [x] [Half-Intrusive hash table(Based on avl-tree)](zda/avl_ht.h)  
相关文档参考[avl_ht.h](zda/avl_ht.h)  
使用方式参考[单元测试文件](test/avl_ht_test.cc)  
* [x] [Double-ended single-linked-list(Delist)](zda/delist.h)  
`Delist`是一个支持O(1)尾插的单链表，通过它可以实现队列。注意，该数据结构并不提供尾删操作，因为单链表不可能零开销实现O(1)的尾删。  
其他操作参考上面提到的单链表。  
相关文档参考[delist.h](zda/delist.h)  
使用方式参考[单元测试文件](test/delist_test.cc)  
* [x] [String View](zda/string_view.h)  
对于 `C++17` 的用户来说，这个并不陌生。所谓StringView是指字符串的一个切片，或者只读视图，这个视图不允许进行写操作，而只能读取其内容。  
在实际的编程中，我们只是利用其内容，因此有些函数，比如 `substr()` 是很容易被误用的，因为他们并没有意识到这个函数可能会造成一次动态分配，而这个动态分配是没必要的，返回一个切片是更合适的做法。  
`StringView` 正是为了这类场景诞生的。  

## C++ wrapper class
The library also provides C++ wrapper class template to make C++ user easy to use. For example,
```cpp
template <typename Entry, typename Free = LibcFree>
class List {
  /* .. */
};
```
If you want to know more, please read the code file that ends with the *.hpp extension.

## Benchmark
TODO...
