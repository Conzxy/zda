#ifndef _ZDA_SLIST_HPP__
#define _ZDA_SLIST_HPP__

#include "zda/iter/slist_iter.hpp"
#include "zda/util/functor.hpp"
#include <unistd.h>
#include <zda/slist.h>

namespace zda {

template <typename Entry, typename Free = LibcFree<Entry>>
class Slist : protected Free {
 public:
    using iterator       = SlistIterator<Entry>;
    using const_iterator = SlistConstIterator<Entry>;
    using entry_type     = Entry;

    Slist() noexcept { zda_slist_header_init(&header_); }
    ~Slist() noexcept { zda_slist_destroy_free(&header_, Entry, (*((Free *)this))); }

    Slist(Slist &&rhs) noexcept { zda_slist_move(&header_, &rhs.header_); }
    Slist &operator=(Slist &&rhs) noexcept
    {
        zda_slist_swap(&header_, &rhs.header_);
        return *this;
    }

    void push_front(zda_slist_node_t *node) noexcept { zda_slist_push_front(&header_, node); }

    void pop_front() noexcept { zda_slist_pop_front(&header_); }

    Entry &front() noexcept { return *zda_slist_entry(zda_slist_front(&header_), Entry); }

    Entry const &front() const noexcept { return static_cast<Slist *>(this)->front(); }

    bool is_empty() const noexcept { return zda_slist_is_empty(&header_); }

    void insert_after(const_iterator pos, zda_slist_node_t *new_node) noexcept
    {
        zda_slist_insert_after(pos.node(), new_node);
    }

    Entry *remove_after(iterator pos) noexcept
    {
        return zda_slist_entry(zda_slist_remove_after(pos.node()), Entry);
    }

    iterator       begin() noexcept { return header_.node.next; }
    const_iterator begin() const noexcept { return header_.node.next; }
    iterator       end() noexcept { return nullptr; }
    const_iterator end() const noexcept { return nullptr; }

    zda_slist_header_t &rep() noexcept { return header_; }

 private:
    zda_slist_header_t header_;
};

} // namespace zda

#endif
