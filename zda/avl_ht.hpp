#ifndef _ZDA_AVL_HT_HPP_
#define _ZDA_AVL_HT_HPP_

#include "zda/avl_ht.h"
#include "zda/util/functor.hpp"
#include "zda/util/map_functor.hpp"
#include "zda/util/comparator.hpp"
#include "zda/iter/avl_ht_iter.hpp"

namespace zda {

#define __ZDA_AVL_HT2HASH (*((Hash *)this))
#define __ZDA_AVL_HT2CMP  (*((Cmp *)this))
#define __ZDA_AVL_HT2GK   (*((GetKey *)this))

template <
    typename Entry,
    typename Key,
    typename GetKey = GetKey<Entry, Key>,
    typename Hash   = std::hash<Key>,
    typename Cmp    = Comparator<Entry>,
    typename Free   = LibcFree<Entry>>
class AvlHt
  : protected GetKey
  , protected Hash
  , protected Cmp
  , protected Free {
 public:
    using entry_type     = Entry;
    using get_key_type   = GetKey;
    using hash_type      = Hash;
    using key_type       = Key;
    using free_type      = Free;
    using iterator       = AvlHtIterator<entry_type>;
    using const_iterator = AvlHtConstIterator<entry_type>;

    using AKey = typename std::conditional<std::is_trivial<Key>::value, Key, Key const &>::type;

    AvlHt() zda_noexcept { zda_avl_ht_init(&ht_); }
    ~AvlHt() zda_noexcept { zda_avl_ht_destroy_inplace(&ht_, entry_type, (*(Free *)this)); }

    bool   is_empty() const zda_noexcept { return zda_avl_ht_is_empty(&ht_); }
    size_t size() const zda_noexcept { return zda_avl_ht_get_count(&ht_); }
    size_t bucket_size() const zda_noexcept { return zda_avl_ht_bucket_count(&ht_); }
    double load_factor() const zda_noexcept { return zda_avl_ht_get_load_factor(&ht_); }

    Entry *insert_check(AKey key, zda_avl_ht_commit_ctx_t *p_ctx) zda_noexcept
    {
        Entry *p_dup;
        zda_avl_ht_insert_check_inplace(
            &ht_,
            key,
            Entry,
            __ZDA_AVL_HT2GK,
            __ZDA_AVL_HT2HASH,
            __ZDA_AVL_HT2CMP,
            *p_ctx,
            p_dup
        );
        return p_dup;
    }

    void insert_commit(zda_avl_ht_commit_ctx_t *p_ctx, zda_avl_ht_node_t *node) zda_noexcept
    {
        zda_avl_ht_insert_commit(&ht_, p_ctx, node);
    }

    Entry *insert_entry(AKey key, Entry *entry) zda_noexcept
    {
        Entry *p_dup;
        zda_avl_ht_insert_entry_inplace(
            &ht_,
            entry,
            Entry,
            __ZDA_AVL_HT2GK,
            __ZDA_AVL_HT2HASH,
            __ZDA_AVL_HT2CMP,
            p_dup
        );
        return p_dup;
    }

    Entry *search(AKey key) zda_noexcept
    {
        Entry *ret;
        zda_avl_ht_search_inplace(
            &ht_,
            key,
            Entry,
            __ZDA_AVL_HT2GK,
            __ZDA_AVL_HT2HASH,
            __ZDA_AVL_HT2CMP,
            ret
        );
        return ret;
    }

    Entry *remove(AKey key) zda_noexcept
    {
        Entry *ret;
        zda_avl_ht_remove_inplace(
            &ht_,
            key,
            Entry,
            __ZDA_AVL_HT2GK,
            __ZDA_AVL_HT2HASH,
            __ZDA_AVL_HT2CMP,
            ret
        );
        return ret;
    }

    iterator       begin() noexcept { return zda_avl_ht_get_first(&ht_); }
    const_iterator begin() const noexcept { return zda_avl_ht_get_first((zda_avl_ht_t *)&ht_); }

    iterator       end() noexcept { return zda_avl_ht_get_terminator(&ht_); }
    const_iterator end() const noexcept { return zda_avl_ht_get_terminator((zda_avl_ht_t *)&ht_); }

 private:
    zda_avl_ht_t ht_;
};

} // namespace zda

#endif
