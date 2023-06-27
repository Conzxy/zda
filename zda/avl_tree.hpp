#ifndef _ZDA_AVL_TREE_HPP__
#define _ZDA_AVL_TREE_HPP__

#include <zda/avl_tree.h>
#include <zda/iter/avl_tree_iter.hpp>
#include <zda/util/comparator.hpp>
#include <zda/util/functor.hpp>

namespace zda {

template <
    typename EntryType,
    typename Key,
    typename GetKey,
    typename Compare = Comparator<Key>,
    typename Free    = LibcFree>
class AvlTree
  : protected Compare
  , protected Free
  , protected GetKey {
 public:
  using entry_type = EntryType;
  using key_type   = Key;
  using iterator   = AvlTreeIterator<EntryType>;
  using get_key    = GetKey;
  using rep_type   = zda_avl_tree_t;

  AvlTree() noexcept;
  ~AvlTree() noexcept;

  EntryType *insert_entry(EntryType *entry) noexcept;
  EntryType *insert_check(Key const &key, zda_avl_commit_ctx_t *p_cmt_ctx) noexcept;
  void       insert_commit(zda_avl_commit_ctx_t *p_cmt_ctx, zda_avl_node_t *node) noexcept;

  EntryType *search(Key const &key) noexcept;

  void       remove_node(zda_avl_node_t *node) noexcept;
  EntryType *remove(Key const &key) noexcept;

  iterator  begin() noexcept { return zda_avl_tree_get_first(&tree_); }
  iterator  last() noexcept { return zda_avl_tree_get_last(&tree_); }
  iterator  end() noexcept { return zda_avl_tree_get_terminator(&tree_); }
  rep_type &rep() noexcept { return tree_; }

 private:
  zda_avl_tree_t tree_;
};

#define _ZDA_AVL_TREE_TEMPLATE_LIST_                                                               \
  template <typename EntryType, typename Key, typename GetKey, typename Compare, typename Free>

#define _ZDA_AVL_TREE_TEMPLATE_CLASS_ AvlTree<EntryType, Key, GetKey, Compare, Free>

#define _ZDA_AVL_TREE_TO_COMPARE_ (*((Compare *)this))
#define _ZDA_AVL_TREE_TO_FREE_    (*((Free *)this))
#define _ZDA_AVL_TREE_TO_GET_KEY_ (*((GetKey *)this))

_ZDA_AVL_TREE_TEMPLATE_LIST_
zda_inline _ZDA_AVL_TREE_TEMPLATE_CLASS_::AvlTree() noexcept { zda_avl_tree_init(&tree_); }

_ZDA_AVL_TREE_TEMPLATE_LIST_
zda_inline _ZDA_AVL_TREE_TEMPLATE_CLASS_::~AvlTree() noexcept
{
  zda_avl_tree_destroy_inplace(&tree_, EntryType, _ZDA_AVL_TREE_TO_FREE_);
}

_ZDA_AVL_TREE_TEMPLATE_LIST_
zda_inline EntryType *_ZDA_AVL_TREE_TEMPLATE_CLASS_::insert_entry(EntryType *entry) noexcept
{
  zda_avl_commit_ctx_t cmt_ctx;
  EntryType           *p_dup;
  zda_avl_tree_insert_check_inplace(
      &tree_,
      _ZDA_AVL_TREE_TO_GET_KEY_(entry),
      EntryType,
      _ZDA_AVL_TREE_TO_GET_KEY_,
      _ZDA_AVL_TREE_TO_COMPARE_,
      cmt_ctx,
      p_dup
  );
  if (p_dup) return p_dup;
  zda_avl_tree_insert_commit(&tree_, &cmt_ctx, &entry->node);
  return entry;
}

_ZDA_AVL_TREE_TEMPLATE_LIST_
EntryType *_ZDA_AVL_TREE_TEMPLATE_CLASS_::insert_check(
    Key const            &key,
    zda_avl_commit_ctx_t *p_cmt_ctx
) noexcept
{
  entry_type *p_dup;
  zda_avl_tree_insert_check_inplace(
      &tree_,
      key,
      EntryType,
      _ZDA_AVL_TREE_TO_GET_KEY_,
      _ZDA_AVL_TREE_TO_COMPARE_,
      *p_cmt_ctx,
      p_dup
  );
  return p_dup;
}

_ZDA_AVL_TREE_TEMPLATE_LIST_
void _ZDA_AVL_TREE_TEMPLATE_CLASS_::insert_commit(
    zda_avl_commit_ctx_t *p_cmt_ctx,
    zda_avl_node_t       *node
) noexcept
{
  zda_avl_tree_insert_commit(&tree_, p_cmt_ctx, node);
}

_ZDA_AVL_TREE_TEMPLATE_LIST_
EntryType *_ZDA_AVL_TREE_TEMPLATE_CLASS_::search(Key const &key) noexcept
{
  EntryType *ret;
  zda_avl_tree_search_inplace(
      &tree_,
      key,
      EntryType,
      _ZDA_AVL_TREE_TO_GET_KEY_,
      _ZDA_AVL_TREE_TO_COMPARE_,
      ret
  );
  return ret;
}

_ZDA_AVL_TREE_TEMPLATE_LIST_
void _ZDA_AVL_TREE_TEMPLATE_CLASS_::remove_node(zda_avl_node_t *node) noexcept
{
  zda_avl_tree_remove_node(&tree_, node);
}

_ZDA_AVL_TREE_TEMPLATE_LIST_
EntryType *_ZDA_AVL_TREE_TEMPLATE_CLASS_::remove(Key const &key) noexcept
{
  EntryType *ret;
  zda_avl_tree_remove_inplace(
      &tree_,
      key,
      entry_type,
      _ZDA_AVL_TREE_TO_GET_KEY_,
      _ZDA_AVL_TREE_TO_COMPARE_,
      ret
  );
  return ret;
}
} // namespace zda

#endif
