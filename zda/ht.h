// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_HT_H__
#define _ZDA_HT_H__

#include "zda/slist.h"
#include "zda/util/macro.h"
#include "zda/util/container_of.h"
#include "zda/util/bool.h"

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

typedef zda_slist_node_t   zda_ht_node_t;
typedef zda_slist_header_t zda_ht_list_t;

#define ZDA_HT_HOOK zda_ht_node_t node

/* Use callback table like the virtual table in C++ is not
 * the optimal solution to provide comparing and hashing */
typedef struct zda_ht {
  zda_ht_list_t *tb;
  size_t         bkt_capa;
  size_t         cnt;
  size_t         mask;
} zda_ht_t;

typedef struct zda_ht_commit_ctx {
  size_t bkt_idx;
} zda_ht_commit_ctx_t;

typedef struct zda_ht_iter {
  zda_ht_t      *ht;
  zda_ht_node_t *node;
  size_t         idx;
} zda_ht_iter_t;

#define zda_ht_entry(p_node, type)    container_of(p_node, type, node)
#define zda_ht_iter2entry(iter, type) zda_ht_entry((iter).node, type)

static zda_inline void zda_ht_init(zda_ht_t *ht) zda_noexcept
{
  ht->cnt = ht->bkt_capa = 0;
  ht->mask               = 0;
  ht->tb                 = NULL;
}

ZDA_API int zda_ht_reserve_init(zda_ht_t *ht, size_t n);

static zda_inline zda_bool zda_ht_is_empty(zda_ht_t const *ht) zda_noexcept { return ht->cnt == 0; }
static zda_inline size_t   zda_ht_get_count(zda_ht_t const *ht) zda_noexcept { return ht->cnt; }
static zda_inline size_t   zda_ht_bucket_count(zda_ht_t const *ht) zda_noexcept
{
  return ht->bkt_capa;
}

static zda_inline int _zda_ht_need_rehash(zda_ht_t *ht) zda_noexcept
{
  return ht->bkt_capa <= ht->cnt;
}

static zda_inline size_t _zda_ht_get_new_capa(zda_ht_t *ht) zda_noexcept
{
  return ht->bkt_capa == 0 ? 1 : (ht->bkt_capa << 1);
}

static zda_inline double zda_ht_get_load_factor(zda_ht_t const *ht) zda_noexcept
{
  return (double)(ht->cnt) / ht->bkt_capa;
}

#define _zda_ht_compute_bkt_idx(ht, key, hash) (hash(key) & ht->mask)

/* To make the hash and compare callback can be inlined intead of a ordinary function call,
 * users should use the following to generate codes or function definitions to achieve it. */

#define _zda_ht_rehash_capa(__ht, type, get_key, hash, new_capa)                                   \
  do {                                                                                             \
    zda_ht_list_t *new_tb = (zda_ht_list_t *)malloc(sizeof(zda_ht_list_t) * new_capa);             \
    for (size_t i = 0; i < new_capa; ++i) {                                                        \
      zda_slist_header_init(&new_tb[i]);                                                           \
    }                                                                                              \
    __ht->mask = new_capa - 1;                                                                     \
    if (__ht->tb) {                                                                                \
      for (size_t i = 0; i < __ht->bkt_capa; ++i) {                                                \
        for (zda_ht_node_t *pos = __ht->tb[i].node.next; pos != NULL;) {                           \
          const size_t   new_idx         = __ht->mask & hash(get_key(zda_ht_entry(pos, type)));    \
          zda_ht_node_t *steal_node_next = pos->next;                                              \
          pos->next                      = new_tb[new_idx].node.next;                              \
          new_tb[new_idx].node.next      = pos;                                                    \
          pos                            = steal_node_next;                                        \
        }                                                                                          \
      }                                                                                            \
    }                                                                                              \
    __ht->bkt_capa = new_capa;                                                                     \
    free(__ht->tb);                                                                                \
    __ht->tb = new_tb;                                                                             \
  } while (0)

#define _zda_ht_rehash(__ht, type, get_key, hash)                                                  \
  do {                                                                                             \
    const size_t new_capa = _zda_ht_get_new_capa(__ht);                                            \
    _zda_ht_rehash_capa(__ht, type, get_key, hash, new_capa);                                      \
  } while (0)

/* User can use these *_inplace function-like macros to execute opertions,
 * but all these statement(or instruction) is inlined, ie. expand inplace.
 * If you don't like it and want it becomes a real function call,
 * you can declare your function and use *_inplace macro in its definition body.
 * e.g.
 * ```C
 * size_t hash(key_type key) { ... }
 * zda_bool cmp(key_type x, key_type key) { ... }
 * zda_bool xxx_insert_check(zda_ht_t *ht, key_type key, zda_ht_commit_ctx *p_ctx,
 *                           entry_type **p_dup)
 * {
 *   zda_bool result;
 *   zda_ht_insert_check_inplace(ht, key, entry_type, key_member, hash, cmp,
 *                               p_ctx, result, *p_dup);
 *   return result;
 * }
 * ```
 */

/**
 * @brief Check whether the key has inserted
 * If the key does exists in the hash table, the insertion is failed.
 * Otherwise, the function return a commit context used for insert a node
 * to hash table.
 * By this way, the node can be allocated out of the insert function.
 * It make the allocation of node is flexible and easy to handle allocation
 * error.
 */
#define zda_ht_insert_check_inplace(ht, key, type, get_key, hash, cmp, commit_ctx, p_dup)          \
  do {                                                                                             \
    /* If user pass `&ht` to `ht` paramenter, I don't want to use temporary object */              \
    zda_ht_t *__ht = ht;                                                                           \
    p_dup          = NULL;                                                                         \
    if (_zda_ht_need_rehash(__ht)) {                                                               \
      _zda_ht_rehash(__ht, type, get_key, hash);                                                   \
    }                                                                                              \
    (commit_ctx).bkt_idx       = hash(key) & __ht->mask;                                           \
    zda_ht_list_t *insert_list = &__ht->tb[(commit_ctx).bkt_idx];                                  \
    for (zda_ht_node_t *pos = insert_list->node.next; pos != NULL; pos = pos->next) {              \
      type *entry = zda_ht_entry(pos, type);                                                       \
      if (cmp(get_key(entry), key)) {                                                              \
        p_dup = entry;                                                                             \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

ZDA_API void zda_ht_insert_commit(zda_ht_t *ht, zda_ht_commit_ctx_t *p_ctx, zda_ht_node_t *node)
    zda_noexcept;

#define zda_ht_insert_commit_inplace(ht, commit_ctx, _node)                                        \
  do {                                                                                             \
    zda_ht_t *__ht = ht;                                                                           \
    assert(!zda_ht_is_empty(__ht));                                                                \
    zda_ht_list_t *p_insert_list = &__ht->tb[(commit_ctx).bkt_idx];                                \
    (_node)->next                = p_insert_list->node.next;                                       \
    p_insert_list->node.next     = (_node);                                                        \
    __ht->cnt++;                                                                                   \
  } while (0)

/**
 * @brief Insert an allocated node to the hash table
 * The node has allocated by caller before call this function.
 * @param ht (zda_ht_t*) Hash table handle
 * @param entry (User-defined type*) The entry that you want to insert
 * @param type Entry type that contains the `zda_ht_node_t` member
 * @param get_key Get the key of entry
 * @param hash Hash function, signature: size_t hash(key_type key)
 * @param cmp Compare function, signature: zda_bool compare(key_type key, key_type key)
 * @param[out] p_dup
 * If the insertion is successful, the \p p_dup pointer to the existed entry
 * otherwise, the p_dup is NULL
 */
#define zda_ht_insert_entry_inplace(ht, entry, type, get_key, hash, cmp, p_dup)                    \
  do {                                                                                             \
    zda_ht_commit_ctx_t commit_ctx;                                                                \
    zda_ht_insert_check_inplace(ht, get_key(entry), type, get_key, hash, cmp, commit_ctx, p_dup);  \
    if (p_dup) break;                                                                              \
    zda_ht_insert_commit_inplace(ht, commit_ctx, &entry->node);                                    \
  } while (0)

#define zda_ht_search_inplace(ht, key, type, get_key, hash, cmp, result_entry)                     \
  do {                                                                                             \
    zda_ht_t *__ht = ht;                                                                           \
    result_entry   = NULL;                                                                         \
    if (zda_ht_is_empty(__ht)) {                                                                   \
      break;                                                                                       \
    }                                                                                              \
    size_t         bkt_idx = _zda_ht_compute_bkt_idx(__ht, key, hash);                             \
    zda_ht_list_t *hlist   = &__ht->tb[bkt_idx];                                                   \
    zda_slist_iterate(hlist)                                                                       \
    {                                                                                              \
      type *entry = zda_ht_entry(pos, type);                                                       \
      if (cmp(get_key(entry), key)) {                                                              \
        result_entry = entry;                                                                      \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#define zda_ht_remove_inplace(ht, key, type, get_key, hash, cmp, o_entry)                          \
  do {                                                                                             \
    zda_ht_t *__ht = ht;                                                                           \
    o_entry        = NULL;                                                                         \
    if (zda_ht_is_empty(__ht)) {                                                                   \
      break;                                                                                       \
    }                                                                                              \
    size_t         bkt_idx = _zda_ht_compute_bkt_idx(__ht, key, hash);                             \
    zda_ht_list_t *hlist   = &(__ht->tb[bkt_idx]);                                                 \
    for (zda_ht_node_t *pos = &hlist->node; pos->next != NULL;) {                                  \
      type *cur_entry = zda_ht_entry(pos->next, type);                                             \
      if (cmp(get_key(cur_entry), key)) {                                                          \
        o_entry   = cur_entry;                                                                     \
        pos->next = o_entry->node.next;                                                            \
        __ht->cnt--;                                                                               \
        break;                                                                                     \
      }                                                                                            \
      pos = pos->next;                                                                             \
    }                                                                                              \
  } while (0)

#define zda_ht_destroy_inplace(ht, entry_type, free_cb)                                            \
  do {                                                                                             \
    zda_ht_t *__ht = ht;                                                                           \
    for (size_t i = 0; i < __ht->bkt_capa; ++i) {                                                  \
      zda_ht_list_t *hlist = &__ht->tb[i];                                                         \
      zda_ht_node_t *first = hlist->node.next;                                                     \
      while (first) {                                                                              \
        hlist->node.next = first->next;                                                            \
        free_cb(zda_ht_entry(first, entry_type));                                                  \
        first = hlist->node.next;                                                                  \
      }                                                                                            \
    }                                                                                              \
    free(__ht->tb);                                                                                \
    __ht->mask = __ht->bkt_capa = __ht->cnt = 0;                                                   \
  } while (0)

/**********************************/
/* Iterator APIs */
/**********************************/

static zda_inline zda_ht_iter_t _zda_ht_mk_iter(zda_ht_t *ht, zda_ht_node_t *node, size_t idx)
    zda_noexcept
{
  zda_ht_iter_t iter;
  iter.ht   = ht;
  iter.node = node;
  iter.idx  = idx;
  return iter;
}

static zda_inline int zda_ht_iter_is_terminator(zda_ht_iter_t *iter) zda_noexcept
{
  return iter->node == NULL;
}

static zda_inline zda_ht_iter_t zda_ht_get_terminator(zda_ht_t *ht) zda_noexcept
{
  zda_ht_iter_t iter;
  iter.ht   = ht;
  iter.node = NULL;
  return iter;
}

ZDA_API zda_ht_iter_t zda_ht_get_first(zda_ht_t *ht) zda_noexcept;

ZDA_API void zda_ht_iter_inc(zda_ht_iter_t *iter) zda_noexcept;

/************************************/
/* Debug APIs */
/************************************/
ZDA_API void zda_ht_print_layout(zda_ht_t *ht, void (*print_cb)(zda_ht_node_t *node)) zda_noexcept;

/************************************/
/* Wrapper macro */
/************************************/
#define zda_decl_ht_insert_check(func_name, key_type, entry_type)                                  \
  entry_type *func_name(zda_ht_t *ht, key_type key, zda_ht_commit_ctx_t *p_ctx) zda_noexcept

#define zda_def_ht_insert_check(func_name, key_type, entry_type, get_key, hash, cmp)               \
  zda_decl_ht_insert_check(func_name, key_type, entry_type)                                        \
  {                                                                                                \
    entry_type *p_dup;                                                                             \
    zda_ht_insert_check_inplace(ht, key, entry_type, get_key, hash, cmp, *p_ctx, p_dup);           \
    return p_dup;                                                                                  \
  }

#define zda_decl_ht_insert_commit(func_name, entry_type)                                           \
  void func_name(zda_ht_t *ht, zda_ht_commit_ctx_t *cmt_ctx, entry_type *p_entry)

#define zda_def_ht_insert_commit(func_name, entry_type)                                            \
  zda_decl_ht_insert_commit(func_name, entry_type)                                                 \
  {                                                                                                \
    zda_ht_node_t *node = &p_entry->node;                                                          \
    zda_ht_insert_commit_inplace(ht, *cmt_ctx, node);                                              \
  }

#define zda_decl_ht_search(func_name, key_type, entry_type)                                        \
  entry_type *func_name(zda_ht_t *ht, key_type key) zda_noexcept

#define zda_def_ht_search(func_name, key_type, entry_type, get_key, hash, cmp)                     \
  zda_decl_ht_search(func_name, key_type, entry_type)                                              \
  {                                                                                                \
    entry_type *result;                                                                            \
    zda_ht_search_inplace(ht, key, entry_type, get_key, hash, cmp, result);                        \
    return result;                                                                                 \
  }

#define zda_decl_ht_remove(func_name, key_type, entry_type)                                        \
  entry_type *func_name(zda_ht_t *ht, key_type key) zda_noexcept

#define zda_def_ht_remove(func_name, key_type, entry_type, get_key, hash, cmp)                     \
  zda_decl_ht_remove(func_name, key_type, entry_type)                                              \
  {                                                                                                \
    entry_type *result;                                                                            \
    zda_ht_remove_inplace(ht, key, entry_type, get_key, hash, cmp, result);                        \
    return result;                                                                                 \
  }

#define zda_decl_ht_destroy(func_name) void func_name(zda_ht_t *ht)

#define zda_def_ht_destroy(func_name, entry_type, free_cb)                                         \
  void func_name(zda_ht_t *ht)                                                                     \
  {                                                                                                \
    zda_ht_destroy_inplace(ht, entry_type, free_cb);                                               \
  }

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif /* Header guard */