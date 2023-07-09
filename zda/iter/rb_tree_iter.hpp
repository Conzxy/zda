#ifndef _ZDA_RB_TREE_ITER_HPP__
#define _ZDA_RB_TREE_ITER_HPP__

#include <zda/rb_tree.h>

namespace zda {

template <typename EntryType>
struct RbTreeConstIterator {
 public:
    RbTreeConstIterator(zda_rb_header_t *header, zda_rb_node_t *node) noexcept
      : header_(header)
      , node_(node)
    {
    }

    RbTreeConstIterator &operator++() noexcept
    {
        node_ = zda_rb_node_get_successor(header_, node_);
        return *this;
    }

    RbTreeConstIterator &operator--() noexcept
    {
        node_ = zda_rb_node_get_predecessor(header_, node_);
        return *this;
    }

    RbTreeConstIterator operator++(int) noexcept
    {
        auto ret = *this;
        node_    = zda_rb_node_get_successor(header_, node_);
        return ret;
    }

    RbTreeConstIterator operator--(int) noexcept
    {
        auto ret = *this;
        node_    = zda_rb_node_get_predecessor(header_, node_);
        return ret;
    }

    EntryType const &operator*() noexcept { return *zda_rb_entry(node_, EntryType const); }
    EntryType const &operator*() const noexcept
    {
        return *(static_cast<RbTreeConstIterator *>(this));
    }

    EntryType const *operator->() noexcept { return zda_rb_entry(node_, EntryType const); }
    EntryType const *operator->() const noexcept { return zda_rb_entry(node_, EntryType const); }

    zda_rb_node_t       *node() noexcept { return node_; }
    zda_rb_node_t const *node() const noexcept { return node_; }

    friend zda_inline bool operator==(RbTreeConstIterator lhs, RbTreeConstIterator rhs) noexcept
    {
        return lhs.node_ == rhs.node_;
    }

    friend zda_inline bool operator!=(RbTreeConstIterator lhs, RbTreeConstIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_rb_header_t *header_;
    zda_rb_node_t   *node_;
};

template <typename EntryType>
struct RbTreeIterator {
 public:
    RbTreeIterator(zda_rb_header_t *header, zda_rb_node_t *node) noexcept
      : header_(header)
      , node_(node)
    {
    }

    operator RbTreeIterator<EntryType>() noexcept
    {
        return RbTreeIterator<EntryType>(header_, node_);
    }

    RbTreeIterator &operator++() noexcept
    {
        node_ = zda_rb_node_get_successor(header_, node_);
        return *this;
    }

    RbTreeIterator &operator--() noexcept
    {
        node_ = zda_rb_node_get_predecessor(header_, node_);
        return *this;
    }

    RbTreeIterator operator++(int) noexcept
    {
        auto ret = *this;
        node_    = zda_rb_node_get_successor(header_, node_);
        return ret;
    }

    RbTreeIterator operator--(int) noexcept
    {
        auto ret = *this;
        node_    = zda_rb_node_get_predecessor(header_, node_);
        return ret;
    }

    EntryType       &operator*() noexcept { return *zda_rb_entry(node_, EntryType); }
    EntryType const &operator*() const noexcept { return *(static_cast<RbTreeIterator *>(this)); }

    EntryType       *operator->() noexcept { return zda_rb_entry(node_, EntryType); }
    EntryType const *operator->() const noexcept { return zda_rb_entry(node_, EntryType); }

    zda_rb_node_t       *node() noexcept { return node_; }
    zda_rb_node_t const *node() const noexcept { return node_; }

    friend zda_inline bool operator==(RbTreeIterator lhs, RbTreeIterator rhs) noexcept
    {
        return lhs.node_ == rhs.node_;
    }

    friend zda_inline bool operator!=(RbTreeIterator lhs, RbTreeIterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

 private:
    zda_rb_header_t *header_;
    zda_rb_node_t   *node_;
};

} // namespace zda

#endif /* guard */
