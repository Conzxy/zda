#ifndef _ZDA_AVL_TREE_ITER_HPP__
#define _ZDA_AVL_TREE_ITER_HPP__

#include "zda/avl_tree.h"

namespace zda {

template <typename EntryType>
struct AvlTreeIterator {
 public:
  AvlTreeIterator(zda_avl_node_t *node) noexcept
    : node_(node)
  {
  }

  AvlTreeIterator &operator++() noexcept
  {
    node_ = zda_avl_node_get_next(node_);
    return *this;
  }

  AvlTreeIterator &operator--() noexcept
  {
    node_ = zda_avl_node_get_prev(node_);
    return *this;
  }

  AvlTreeIterator operator++(int) noexcept
  {
    auto ret = *this;
    node_    = zda_avl_node_get_next(node_);
    return ret;
  }

  AvlTreeIterator operator--(int) noexcept
  {
    auto ret = *this;
    node_    = zda_avl_node_get_prev(node_);
    return ret;
  }

  EntryType       &operator*() noexcept { return *zda_avl_entry(node_, EntryType); }
  EntryType const &operator*() const noexcept { return *(static_cast<AvlTreeIterator *>(this)); }

  EntryType       *operator->() noexcept { return zda_avl_entry(node_, EntryType); }
  EntryType const *operator->() const noexcept { return zda_avl_entry(node_, EntryType); }

 private:
  zda_avl_node_t *node_;
};

} // namespace zda

#endif
