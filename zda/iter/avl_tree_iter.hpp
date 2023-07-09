#ifndef _ZDA_AVL_TREE_ITER_HPP__
#define _ZDA_AVL_TREE_ITER_HPP__

#include "zda/avl_tree.h"

namespace zda {

template <typename EntryType>
struct AvlTreeConstIterator {
 public:
    AvlTreeConstIterator(zda_avl_node_t const *node) noexcept
      : node_((zda_avl_node_t *)node)
    {
    }

    AvlTreeConstIterator &operator++() noexcept
    {
        node_ = zda_avl_node_get_next(node_);
        return *this;
    }

    AvlTreeConstIterator &operator--() noexcept
    {
        node_ = zda_avl_node_get_prev(node_);
        return *this;
    }

    AvlTreeConstIterator operator++(int) noexcept
    {
        auto ret = *this;
        node_    = zda_avl_node_get_next(node_);
        return ret;
    }

    AvlTreeConstIterator operator--(int) noexcept
    {
        auto ret = *this;
        node_    = zda_avl_node_get_prev(node_);
        return ret;
    }

    EntryType const &operator*() noexcept { return *zda_avl_entry(node_, EntryType); }
    EntryType const &operator*() const noexcept
    {
        return *(static_cast<AvlTreeConstIterator *>(this));
    }

    EntryType const *operator->() noexcept { return zda_avl_entry(node_, EntryType); }
    EntryType const *operator->() const noexcept { return zda_avl_entry(node_, EntryType); }

    zda_avl_node_t const *node() const noexcept { return node_; }
    zda_avl_node_t       *node() noexcept { return node_; }

    friend zda_inline bool operator==(AvlTreeConstIterator lhs, AvlTreeConstIterator rhs) noexcept
    {
        return lhs.node_ == rhs.node_;
    }

    friend zda_inline bool operator!=(AvlTreeConstIterator lhs, AvlTreeConstIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_avl_node_t *node_;
};

template <typename EntryType>
struct AvlTreeIterator {
 public:
    AvlTreeIterator(zda_avl_node_t *node) noexcept
      : node_(node)
    {
    }

    operator AvlTreeConstIterator<EntryType>() const noexcept { return node_; }

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

    zda_avl_node_t const *node() const noexcept { return node_; }
    zda_avl_node_t       *node() noexcept { return node_; }

    friend zda_inline bool operator==(AvlTreeIterator lhs, AvlTreeIterator rhs) noexcept
    {
        return lhs.node_ == rhs.node_;
    }

    friend zda_inline bool operator!=(AvlTreeIterator lhs, AvlTreeIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_avl_node_t *node_;
};
} // namespace zda

#endif
