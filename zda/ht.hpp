#ifndef _ZDA_HT_HPP__
#define _ZDA_HT_HPP__

#include "zda/util/functor.hpp"
#include "zda/util/map_functor.hpp"
#include <zda/ht.h>
#include <zda/iter/ht_iter.hpp>
#include <functional>
#include <type_traits>

namespace zda {

template <
    typename Entry,
    typename Key,
    typename GetKey = GetKey<Entry, Key>,
    typename Hash   = std::hash<Key>,
    typename Equal  = std::equal_to<Key>,
    typename Free   = LibcFree<Entry>>
class Ht
  : protected Hash
  , protected Equal
  , protected Free
  , protected GetKey {
 public:
  using entry_type   = Entry;
  using get_key_type = GetKey;
  using hash_type    = Hash;
  using key_type     = Key;
  using equal_type   = Equal;
  using free_type    = Free;
  using iterator     = HtIterator<entry_type>;

  /* Unlike STL, optimize the parameter type of insert/search/remove when key type is trivial type,
   * pass them as value instead of reference to avoid indirect access(and cause cache miss). */
  using AKey = typename std::conditional<std::is_trivial<Key>::value, Key, Key const &>::type;

  Ht() noexcept { zda_ht_init(&ht_); }
  ~Ht() noexcept;

  explicit Ht(size_t n) { zda_ht_reserve_init(&ht_, n); }

  bool   is_empty() const noexcept { return zda_ht_is_empty(&ht_); }
  size_t size() const noexcept { return zda_ht_get_count(&ht_); }
  size_t bucket_size() const noexcept { return zda_ht_bucket_count(&ht_); }
  double load_factor() const noexcept { return zda_ht_get_load_factor(&ht_); }

  Entry *insert_entry(Entry *entry);
  Entry *insert_check(AKey key, zda_ht_commit_ctx_t *p_ctx) noexcept;
  void   insert_commit(zda_ht_commit_ctx_t const *p_ctx, zda_ht_node_t *node);

  Entry *search(AKey key) noexcept;

  Entry *remove(AKey key) noexcept;

  iterator  begin() const noexcept { return zda_ht_get_first((zda_ht_t *)&ht_); }
  iterator  end() const noexcept { return zda_ht_get_terminator((zda_ht_t *)&ht_); }
  zda_ht_t &rep() noexcept { return ht_; }

 private:
  zda_ht_t ht_;
};

#define _ZDA_HT_TEMPLATE_LIST_                                                                     \
  template <                                                                                       \
      typename Entry,                                                                              \
      typename Key,                                                                                \
      typename GetKey,                                                                             \
      typename Hash,                                                                               \
      typename Equal,                                                                              \
      typename Free>

#define _ZDA_HT_TEMPLATE_CLASS_ Ht<Entry, Key, GetKey, Hash, Equal, Free>
#define _ZDA_HT_TO_GET_KEY_     (*((GetKey *)this))
#define _ZDA_HT_TO_HASH_        (*((Hash *)this))
#define _ZDA_HT_TO_EQUAL_       (*((Equal *)this))

_ZDA_HT_TEMPLATE_LIST_
_ZDA_HT_TEMPLATE_CLASS_::~Ht() noexcept { zda_ht_destroy_inplace(&ht_, Entry, (*((Free *)this))); }

_ZDA_HT_TEMPLATE_LIST_
Entry *_ZDA_HT_TEMPLATE_CLASS_::insert_entry(Entry *entry)
{
  Entry *ret;
  zda_ht_insert_entry_inplace(
      &ht_,
      entry,
      Entry,
      _ZDA_HT_TO_GET_KEY_,
      _ZDA_HT_TO_HASH_,
      _ZDA_HT_TO_EQUAL_,
      ret
  );
  return ret;
}

_ZDA_HT_TEMPLATE_LIST_
Entry *_ZDA_HT_TEMPLATE_CLASS_::insert_check(AKey key, zda_ht_commit_ctx_t *p_ctx) noexcept
{
  Entry *p_dup;
  zda_ht_insert_check_inplace(
      &ht_,
      key,
      Entry,
      _ZDA_HT_TO_GET_KEY_,
      _ZDA_HT_TO_HASH_,
      _ZDA_HT_TO_EQUAL_,
      *p_ctx,
      p_dup
  );
  return p_dup;
}

_ZDA_HT_TEMPLATE_LIST_
void _ZDA_HT_TEMPLATE_CLASS_::insert_commit(zda_ht_commit_ctx_t const *p_ctx, zda_ht_node_t *node)
{
  zda_ht_insert_commit_inplace(&ht_, *p_ctx, node);
}

_ZDA_HT_TEMPLATE_LIST_
Entry *_ZDA_HT_TEMPLATE_CLASS_::search(AKey key) noexcept
{
  Entry *ret;
  zda_ht_search_inplace(
      &ht_,
      key,
      Entry,
      _ZDA_HT_TO_GET_KEY_,
      _ZDA_HT_TO_HASH_,
      _ZDA_HT_TO_EQUAL_,
      ret
  );
  return ret;
}

_ZDA_HT_TEMPLATE_LIST_
Entry *_ZDA_HT_TEMPLATE_CLASS_::remove(AKey key) noexcept
{
  Entry *ret;
  zda_ht_remove_inplace(
      &ht_,
      key,
      Entry,
      _ZDA_HT_TO_GET_KEY_,
      _ZDA_HT_TO_HASH_,
      _ZDA_HT_TO_EQUAL_,
      ret
  );
  return ret;
}

} // namespace zda

#endif
