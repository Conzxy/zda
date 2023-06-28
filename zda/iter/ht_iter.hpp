#ifndef _ZDA_HT_ITER_HPP__
#define _ZDA_HT_ITER_HPP__

#include <zda/ht.h>

namespace zda {

template <typename EntryType>
struct HtIterator {
  HtIterator(zda_ht_iter const &iter) noexcept
    : iter_(iter)
  {
  }

  HtIterator &operator++() noexcept
  {
    zda_ht_iter_inc(&iter_);
    return *this;
  }

  HtIterator operator++(int) noexcept
  {
    auto ret = *this;
    zda_ht_iter_inc(&iter_);
    return ret;
  }

  EntryType       &operator*() noexcept { return *zda_ht_entry(iter_.node, EntryType); }
  EntryType const &operator*() const noexcept { return *(static_cast<HtIterator *>(this)); }

  EntryType       *operator->() noexcept { return zda_ht_entry(iter_.node, EntryType); }
  EntryType const *operator->() const noexcept { return zda_ht_entry(iter_.node, EntryType); }

  friend zda_inline bool operator==(HtIterator lhs, HtIterator rhs) noexcept
  {
    return lhs.node_ == rhs.node_;
  }

  friend zda_inline bool operator!=(HtIterator lhs, HtIterator rhs) noexcept { return lhs != rhs; }

 private:
  zda_ht_iter_t iter_;
};

} // namespace zda

#endif