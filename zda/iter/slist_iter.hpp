#ifndef _ZDA_SLIT_ITER_HPP__
#define _ZDA_SLIT_ITER_HPP__

#include "zda/slist.h"

namespace zda {

template <typename EntryType>
struct SlistIterator {
 public:
  SlistIterator(zda_slist_node_t *node) noexcept
    : node_(node)
  {
  }

  SlistIterator &operator++() noexcept
  {
    node_ = node_->next;
    return *this;
  }

  SlistIterator operator++(int) noexcept
  {
    auto ret = *this;
    node_    = node_->next;
    return ret;
  }

  EntryType       &operator*() noexcept { return *zda_slist_entry(node_, EntryType); }
  EntryType const &operator*() const noexcept { return *(static_cast<SlistIterator *>(this)); }

  EntryType       *operator->() noexcept { return zda_slist_entry(node_, EntryType); }
  EntryType const *operator->() const noexcept { return zda_slist_entry(node_, EntryType); }

  friend zda_inline bool operator==(SlistIterator lhs, SlistIterator rhs) noexcept
  {
    return lhs.node_ == rhs.node_;
  }
  friend zda_inline bool operator!=(SlistIterator lhs, SlistIterator rhs) noexcept
  {
    return lhs.node_ != rhs.node_;
  }

 private:
  zda_slist_node_t *node_;
};

} // namespace zda

#endif