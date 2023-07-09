#ifndef __ZDA_AVL_HT_ITER_HPP__
#define __ZDA_AVL_HT_ITER_HPP__

#include "zda/avl_ht.h"

namespace zda {

template <typename EntryType>
class AvlHtConstIterator {
 public:
    AvlHtConstIterator(zda_avl_ht_iter const &iter) noexcept
      : iter_(iter)
    {
    }

    AvlHtConstIterator &operator++() noexcept
    {
        zda_avl_ht_iter_inc(&iter_);
        return *this;
    }

    AvlHtConstIterator operator++(int) noexcept
    {
        auto ret = *this;
        zda_avl_ht_iter_inc(&iter_);
        return ret;
    }

    EntryType const &operator*() noexcept { return *zda_avl_ht_entry(iter_.node, EntryType const); }
    EntryType const &operator*() const noexcept
    {
        return *(static_cast<AvlHtConstIterator *>(this));
    }

    EntryType const *operator->() noexcept { return zda_avl_ht_entry(iter_.node, EntryType const); }
    EntryType const *operator->() const noexcept
    {
        return zda_avl_ht_entry(iter_.node, EntryType const);
    }

    friend zda_inline bool operator==(AvlHtConstIterator lhs, AvlHtConstIterator rhs) noexcept
    {
        return lhs.iter_.node == rhs.iter_.node;
    }

    friend zda_inline bool operator!=(AvlHtConstIterator lhs, AvlHtConstIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_avl_ht_iter_t iter_;
};

template <typename EntryType>
class AvlHtIterator {
 public:
    AvlHtIterator(zda_avl_ht_iter const &iter) noexcept
      : iter_(iter)
    {
    }

    operator AvlHtConstIterator<EntryType>() zda_noexcept { return iter_; }

    AvlHtIterator &operator++() noexcept
    {
        zda_avl_ht_iter_inc(&iter_);
        return *this;
    }

    AvlHtIterator operator++(int) noexcept
    {
        auto ret = *this;
        zda_avl_ht_iter_inc(&iter_);
        return ret;
    }

    EntryType       &operator*() noexcept { return *zda_avl_ht_entry(iter_.node, EntryType); }
    EntryType const &operator*() const noexcept { return *(static_cast<AvlHtIterator *>(this)); }

    EntryType       *operator->() noexcept { return zda_avl_ht_entry(iter_.node, EntryType); }
    EntryType const *operator->() const noexcept { return zda_avl_ht_entry(iter_.node, EntryType); }

    friend zda_inline bool operator==(AvlHtIterator lhs, AvlHtIterator rhs) noexcept
    {
        return lhs.iter_.node == rhs.iter_.node;
    }

    friend zda_inline bool operator!=(AvlHtIterator lhs, AvlHtIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_avl_ht_iter_t iter_;
};

} // namespace zda

#endif
