#ifndef _ZDA_LIST_ITER_HPP__
#define _ZDA_LIST_ITER_HPP__

#include "zda/list.h"

namespace zda {

template <typename EntryType>
struct ListConstIterator {
 public:
    ListConstIterator(zda_list_node_t const *node) noexcept
      : node_((zda_list_node_t *)node)
    {
    }

    ListConstIterator &operator++() noexcept
    {
        node_ = node_->next;
        return *this;
    }

    ListConstIterator operator++(int) noexcept
    {
        auto ret = *this;
        node_    = node_->next;
        return ret;
    }

    ListConstIterator &operator--() noexcept
    {
        node_ = node_->prev;
        return *this;
    }

    ListConstIterator operator--(int) noexcept
    {
        auto ret = *this;
        node_    = node_->prev;
        return ret;
    }

    EntryType const &operator*() noexcept { return *zda_list_entry(node_, EntryType const); }
    EntryType const &operator*() const noexcept
    {
        return *(static_cast<ListConstIterator *>(this));
    }

    EntryType const *operator->() noexcept { return zda_list_entry(node_, EntryType const); }
    EntryType const *operator->() const noexcept { return zda_list_entry(node_, EntryType const); }

    zda_list_node_t       *node() noexcept { return node_; }
    zda_list_node_t const *node() const noexcept { return node_; }

    friend zda_inline bool operator==(ListConstIterator lhs, ListConstIterator rhs) noexcept
    {
        return lhs.node_ == rhs.node_;
    }

    friend zda_inline bool operator!=(ListConstIterator lhs, ListConstIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_list_node_t *node_;
};

template <typename EntryType>
struct ListIterator {
 public:
    ListIterator(zda_list_node_t *node) noexcept
      : node_(node)
    {
    }

    operator ListConstIterator<EntryType>() const noexcept { return node_; }

    ListIterator &operator++() noexcept
    {
        node_ = node_->next;
        return *this;
    }

    ListIterator operator++(int) noexcept
    {
        auto ret = *this;
        node_    = node_->next;
        return ret;
    }

    ListIterator &operator--() noexcept
    {
        node_ = node_->prev;
        return *this;
    }

    ListIterator operator--(int) noexcept
    {
        auto ret = *this;
        node_    = node_->prev;
        return ret;
    }

    EntryType       &operator*() noexcept { return *zda_list_entry(node_, EntryType); }
    EntryType const &operator*() const noexcept { return *(static_cast<ListIterator *>(this)); }

    EntryType       *operator->() noexcept { return zda_list_entry(node_, EntryType); }
    EntryType const *operator->() const noexcept { return zda_list_entry(node_, EntryType); }

    zda_list_node_t       *node() noexcept { return node_; }
    zda_list_node_t const *node() const noexcept { return node_; }

    friend zda_inline bool operator==(ListIterator lhs, ListIterator rhs) noexcept
    {
        return lhs.node_ == rhs.node_;
    }

    friend zda_inline bool operator!=(ListIterator lhs, ListIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_list_node_t *node_;
};

} // namespace zda

#endif
