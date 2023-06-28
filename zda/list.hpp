#ifndef _ZDA_LIST_HPP__
#define _ZDA_LIST_HPP__

#include "zda/util/functor.hpp"
#include "zda/iter/list_iter.hpp"
#include "zda/list.h"

namespace zda {

template <typename Entry, typename Free = LibcFree<Entry>>
class List : protected Free {
 public:
  using iterator   = ListIterator<Entry>;
  using entry_type = Entry;

  List() noexcept { zda_list_header_init(&header_); }
  ~List() noexcept { zda_list_destroy(&header_, Entry, (*((Free *)this))); }

  bool is_empty() const noexcept { return zda_list_is_empty(&header_); }

  Entry       &front() noexcept { return *zda_list_front(&header_); }
  Entry const &front() const noexcept { return *zda_list_front((zda_list_header_t *)&header_); }

  Entry       &back() noexcept { return *zda_list_back(&header_); }
  Entry const &back() const noexcept { return *zda_list_back((zda_list_header_t *)&header_); }

  void insert_before(iterator pos, zda_list_node_t *node) noexcept
  {
    zda_list_insert_before(pos.node(), node);
  }

  void insert_after(iterator pos, zda_list_node_t *node) noexcept
  {
    zda_list_insert_after(pos.node(), node);
  }

  void push_front(zda_list_node_t *node) noexcept { zda_list_push_front(&header_, node); }

  void push_back(zda_list_node_t *node) noexcept { zda_list_push_back(&header_, node); }

  void pop_front() noexcept { zda_list_pop_front(&header_); }

  void pop_back() noexcept { zda_list_pop_back(&header_); }

  void remove(iterator iter) noexcept { zda_list_remove(iter.node()); }

  iterator begin() const noexcept { return header_.node.next; }
  iterator end() noexcept { return &header_.node; }

  zda_list_header_t &rep() noexcept { return header_; }

 private:
  zda_list_header_t header_;
};

} // namespace zda

#endif
