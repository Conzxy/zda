#ifndef _ZDA_HT_ITER_HPP__
#define _ZDA_HT_ITER_HPP__

#include <zda/ht.h>

namespace zda {

template <typename EntryType>
struct HtConstIterator {
    HtConstIterator(zda_ht_iter const &iter) noexcept
      : iter_(iter)
    {
    }

    HtConstIterator &operator++() noexcept
    {
        zda_ht_iter_inc(&iter_);
        return *this;
    }

    HtConstIterator operator++(int) noexcept
    {
        auto ret = *this;
        zda_ht_iter_inc(&iter_);
        return ret;
    }

    EntryType const &operator*() noexcept { return *zda_ht_entry(iter_.node, EntryType const); }
    EntryType const &operator*() const noexcept { return *(static_cast<HtConstIterator *>(this)); }

    EntryType const *operator->() noexcept { return zda_ht_entry(iter_.node, EntryType const); }
    EntryType const *operator->() const noexcept
    {
        return zda_ht_entry(iter_.node, EntryType const);
    }

    friend zda_inline bool operator==(HtConstIterator lhs, HtConstIterator rhs) noexcept
    {
        return lhs.iter_.node == rhs.iter_.node;
    }

    friend zda_inline bool operator!=(HtConstIterator lhs, HtConstIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_ht_iter_t iter_;
};

template <typename EntryType>
struct HtIterator {
    HtIterator(zda_ht_iter const &iter) noexcept
      : iter_(iter)
    {
    }

    operator HtConstIterator<EntryType>() const noexcept { return iter_; }

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
        return lhs.iter_.node == rhs.iter_.node;
    }

    friend zda_inline bool operator!=(HtIterator lhs, HtIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_ht_iter_t iter_;
};

} // namespace zda

#endif
