#ifndef _ZDA_DELIST_HPP__
#define _ZDA_DELIST_HPP__

#include "zda/delist.h"
#include "zda/util/functor.hpp"

#include "zda/iter/slist_iter.hpp"

namespace zda {

template <typename Entry, typename Free = LibcFree<Entry>>
class Delist : protected Free {
 public:
    /* Reuse the slist iterator */
    using iterator       = SlistIterator<Entry>;
    using const_iterator = SlistConstIterator<Entry>;

    Delist() zda_noexcept { zda_delist_init(&dl_); }
    ~Delist() zda_noexcept { zda_delist_destroy_inplace(&dl_, Entry, (*(Free *)(this))); }

    Delist(Delist &&rhs) zda_noexcept { zda_delist_move(&dl_, &rhs.dl_); }
    Delist &operator=(Delist &&rhs) zda_noexcept
    {
        zda_delist_swap(&dl_, &rhs.dl_);
        return *this;
    }

    bool is_empty() const zda_noexcept { return zda_delist_is_empty(&dl_); }
    bool is_single() const zda_noexcept { return zda_delist_is_single(&dl_); }

    Entry const *front() const zda_noexcept { return zda_delist_front_const(&dl_); }
    Entry       *front() zda_noexcept { return zda_delist_front(&dl_); }

    Entry const *back() const zda_noexcept { return zda_delist_back_const(&dl_); }
    Entry       *back() zda_noexcept { return zda_delist_back(&dl_); }

    void insert_after(const_iterator pos, zda_delist_node_t *node) zda_noexcept
    {
        zda_delist_insert_after(&dl_, pos.node(), node);
    }
    void push_front(zda_delist_node_t *node) zda_noexcept { zda_delist_push_front(&dl_, node); }
    void push_back(zda_delist_node_t *node) zda_noexcept { zda_delist_push_back(&dl_, node); }

    Entry *remove_after(const_iterator pos) zda_noexcept
    {
        return zda_delist_entry(zda_delist_remove_after(&dl_, pos.node()), Entry);
    }
    zda_delist_node_t *pop_front() zda_noexcept { return zda_delist_pop_front(&dl_); }

    const_iterator begin() const noexcept { return zda_delist_get_first_const(&dl_); }
    iterator       begin() noexcept { return zda_delist_get_first(&dl_); }

    const_iterator end() const noexcept { return zda_delist_get_terminator_const(&dl_); }
    iterator       end() zda_noexcept { return zda_delist_get_terminator(&dl_); }

 private:
    zda_delist_t dl_;
};

} // namespace zda

#endif
