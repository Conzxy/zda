#ifndef _ZDA_BST_HT_H__
#define _ZDA_BST_HT_H__

#include "zda/avl_tree.h"

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

typedef zda_avl_node_t zda_avl_ht_node_t;
typedef zda_avl_tree_t zda_avl_ht_list_t;

#define ZDA_AVL_HT_HOOK zda_avl_ht_node_t node

typedef struct zda_avl_ht {
    zda_avl_ht_list_t *tb;
    size_t             bkt_capa;
    size_t             cnt;
    size_t             mask;
} zda_avl_ht_t;

typedef struct zda_avl_ht_commit_ctx {
    size_t               bkt_idx;
    zda_avl_commit_ctx_t avl_ctx;
} zda_avl_ht_commit_ctx_t;

typedef struct zda_avl_ht_iter {
    zda_avl_ht_t      *ht;
    zda_avl_ht_node_t *node;
    size_t             idx;
} zda_avl_ht_iter_t;

#define zda_avl_ht_entry(p_node, type)    container_of(p_node, type, node)
#define zda_avl_ht_iter2entry(iter, type) zda_avl_ht_entry((iter).node, type)

/**************************/
/* Initializer */
/**************************/
static zda_inline void zda_avl_ht_init(zda_avl_ht_t *ht) zda_noexcept
{
    ht->cnt = ht->bkt_capa = 0;
    ht->mask               = 0;
    ht->tb                 = NULL;
}

/*************************/
/* Properties getter */
/*************************/
static zda_inline zda_bool zda_avl_ht_is_empty(zda_avl_ht_t const const *ht) zda_noexcept
{
    return ht->cnt == 0;
}
static zda_inline size_t zda_avl_ht_get_count(zda_avl_ht_t const *ht) zda_noexcept
{
    return ht->cnt;
}
static zda_inline size_t zda_avl_ht_bucket_count(zda_avl_ht_t const *ht) zda_noexcept
{
    return ht->bkt_capa;
}

static zda_inline double zda_avl_ht_get_load_factor(zda_avl_ht_t const *ht) zda_noexcept
{
    return (double)(ht->cnt) / ht->bkt_capa;
}
/****************************/
/* Rehash Helper */
/****************************/
static zda_inline int _zda_avl_ht_need_rehash(zda_avl_ht_t *ht) zda_noexcept
{
    return ht->bkt_capa <= ht->cnt;
}

static zda_inline size_t _zda_avl_ht_get_new_capa(zda_avl_ht_t *ht) zda_noexcept
{
    return ht->bkt_capa == 0 ? 1 : (ht->bkt_capa << 1);
}

#define _zda_avl_ht_compute_bkt_idx(ht, key, hash) (hash(key) & ht->mask)

/* To make the hash and compare callback can be inlined intead of a ordinary function call,
 * users should use the following to generate codes or function definitions to achieve it. */

#define _zda_avl_ht_rehash_capa(ht, type, get_key, hash, cmp_cb, new_capa)                         \
    do {                                                                                           \
        zda_avl_ht_list_t *new_tb =                                                                \
            (zda_avl_ht_list_t *)malloc(sizeof(zda_avl_ht_list_t) * new_capa);                     \
        for (size_t i = 0; i < new_capa; ++i) {                                                    \
            zda_avl_tree_init(&new_tb[i]);                                                         \
        }                                                                                          \
        ht->mask = new_capa - 1;                                                                   \
        if (ht->tb) {                                                                              \
            for (size_t i = 0; i < ht->bkt_capa; ++i) {                                            \
                zda_avl_ht_list_t *p_old_list = &ht->tb[i];                                        \
                zda_avl_node_t    *root       = p_old_list->node;                                  \
                type              *entry;                                                          \
                size_t             new_idx;                                                        \
                zda_avl_ht_list_t *new_ht_list;                                                    \
                type              *p_dup;                                                          \
                while (root) {                                                                     \
                    if (root->left) {                                                              \
                        root = root->left;                                                         \
                    } else if (root->right) {                                                      \
                        root = root->right;                                                        \
                    } else {                                                                       \
                        zda_avl_node_t *parent = root->parent;                                     \
                        if (parent) {                                                              \
                            if (parent->left == root) {                                            \
                                parent->left = NULL;                                               \
                            } else {                                                               \
                                assert(parent->right == root);                                     \
                                parent->right = NULL;                                              \
                            }                                                                      \
                        }                                                                          \
                        entry   = zda_avl_ht_entry(root, type);                                    \
                        new_idx = hash(get_key(entry)) & (ht->mask);                               \
                        printf("new_idx = %zu\n", new_idx);                                        \
                        new_ht_list = &new_tb[new_idx];                                            \
                        zda_avl_tree_insert_entry_inplace(                                         \
                            new_ht_list,                                                           \
                            entry,                                                                 \
                            type,                                                                  \
                            get_key,                                                               \
                            cmp_cb,                                                                \
                            p_dup                                                                  \
                        );                                                                         \
                        root = parent;                                                             \
                    }                                                                              \
                }                                                                                  \
            }                                                                                      \
        }                                                                                          \
        ht->bkt_capa = new_capa;                                                                   \
        free(ht->tb);                                                                              \
        ht->tb = new_tb;                                                                           \
    } while (0)

#define _zda_avl_ht_rehash(ht, type, get_key, hash, cmp_cb)                                        \
    do {                                                                                           \
        const size_t new_capa = _zda_avl_ht_get_new_capa(ht);                                      \
        _zda_avl_ht_rehash_capa(ht, type, get_key, hash, cmp_cb, new_capa);                        \
    } while (0)

/***********************************/
/* Insert APIs */
/***********************************/

/**
 * @brief Check whether the key has inserted
 * If the key does exists in the hash table, the insertion is failed.
 * Otherwise, the function return a commit context used for insert a node
 * to hash table.
 * By this way, the node can be allocated out of the insert function.
 * It make the allocation of node is flexible and easy to handle allocation
 * error.
 */
#define zda_avl_ht_insert_check_inplace(ht, key, type, get_key, hash, cmp, commit_ctx, p_dup)      \
    do {                                                                                           \
        /* If user pass `&ht` to `ht` paramenter, I don't want to use temporary object */          \
        zda_avl_ht_t *__ht = ht;                                                                   \
        p_dup              = NULL;                                                                 \
        if (_zda_avl_ht_need_rehash(__ht)) {                                                       \
            _zda_avl_ht_rehash(__ht, type, get_key, hash, cmp);                                    \
        }                                                                                          \
        (commit_ctx).bkt_idx           = hash(key) & __ht->mask;                                   \
        zda_avl_ht_list_t *insert_list = &__ht->tb[(commit_ctx).bkt_idx];                          \
        zda_avl_tree_insert_check_inplace(                                                         \
            insert_list,                                                                           \
            key,                                                                                   \
            type,                                                                                  \
            get_key,                                                                               \
            cmp,                                                                                   \
            (commit_ctx).avl_ctx,                                                                  \
            p_dup                                                                                  \
        );                                                                                         \
    } while (0)

static zda_inline void zda_avl_ht_insert_commit(
    zda_avl_ht_t            *ht,
    zda_avl_ht_commit_ctx_t *p_ctx,
    zda_avl_ht_node_t       *node
) zda_noexcept
{
    zda_avl_ht_list_t *hlist = &ht->tb[p_ctx->bkt_idx];
    zda_avl_tree_insert_commit(hlist, &p_ctx->avl_ctx, node);
    ht->cnt++;
}

/**
 * @brief Insert an allocated node to the hash table
 * The node has allocated by caller before call this function.
 * @param ht (zda_avl_ht_t*) Hash table handle
 * @param entry (User-defined type*) The entry that you want to insert
 * @param type Entry type that contains the `zda_avl_ht_node_t` member
 * @param get_key Get the key of entry
 * @param hash Hash function, signature: size_t hash(key_type key)
 * @param cmp Compare function, signature: zda_bool compare(key_type key, key_type key)
 * @param[out] p_dup
 * If the insertion is successful, the \p p_dup pointer to the existed entry
 * otherwise, the p_dup is NULL
 */
#define zda_avl_ht_insert_entry_inplace(ht, entry, type, get_key, hash, cmp, p_dup)                \
    do {                                                                                           \
        zda_avl_ht_commit_ctx_t commit_ctx;                                                        \
        zda_avl_ht_insert_check_inplace(                                                           \
            ht,                                                                                    \
            get_key(entry),                                                                        \
            type,                                                                                  \
            get_key,                                                                               \
            hash,                                                                                  \
            cmp,                                                                                   \
            commit_ctx,                                                                            \
            p_dup                                                                                  \
        );                                                                                         \
        if (p_dup) break;                                                                          \
        zda_avl_ht_insert_commit(ht, &commit_ctx, &entry->node);                                   \
    } while (0)

/************************************/
/* Search APIs */
/************************************/
#define zda_avl_ht_search_inplace(ht, key, type, get_key, hash, cmp, result_entry)                 \
    do {                                                                                           \
        zda_avl_ht_t *__ht = ht;                                                                   \
        result_entry       = NULL;                                                                 \
        if (zda_avl_ht_is_empty(__ht)) {                                                           \
            break;                                                                                 \
        }                                                                                          \
        size_t             bkt_idx = _zda_avl_ht_compute_bkt_idx(__ht, key, hash);                 \
        zda_avl_ht_list_t *hlist   = &__ht->tb[bkt_idx];                                           \
        zda_avl_tree_search_inplace(hlist, key, type, get_key, cmp, result_entry);                 \
    } while (0)

/****************************************/
/* Remove APIs  */
/****************************************/
#define zda_avl_ht_remove_inplace(ht, key, type, get_key, hash, cmp, o_entry)                      \
    do {                                                                                           \
        zda_avl_ht_t *__ht = ht;                                                                   \
        if (zda_avl_ht_is_empty(__ht)) {                                                           \
            break;                                                                                 \
        }                                                                                          \
        size_t             bkt_idx = _zda_avl_ht_compute_bkt_idx(__ht, key, hash);                 \
        zda_avl_ht_list_t *hlist   = &(__ht->tb[bkt_idx]);                                         \
        zda_avl_tree_remove_inplace(hlist, key, type, get_key, cmp, o_entry);                      \
    } while (0)

/********************************/
/* Destroy API */
/********************************/

#define zda_avl_ht_destroy_inplace(ht, entry_type, free_cb)                                        \
    do {                                                                                           \
        zda_avl_ht_t *__ht = ht;                                                                   \
        for (size_t i = 0; i < __ht->bkt_capa; ++i) {                                              \
            zda_avl_ht_list_t *hlist = &__ht->tb[i];                                               \
            zda_avl_tree_destroy_inplace(hlist, entry_type, free_cb);                              \
        }                                                                                          \
        free(__ht->tb);                                                                            \
        __ht->mask = __ht->bkt_capa = __ht->cnt = 0;                                               \
    } while (0)

/**********************************/
/* Iterator APIs */
/**********************************/

static zda_inline zda_avl_ht_iter_t
_zda_avl_ht_mk_iter(zda_avl_ht_t *ht, zda_avl_ht_node_t *node, size_t idx)
{
    zda_avl_ht_iter_t iter;
    iter.ht   = ht;
    iter.node = node;
    iter.idx  = idx;
    return iter;
}

static zda_inline int zda_avl_ht_iter_is_terminator(zda_avl_ht_iter_t *iter)
{
    return iter->node == NULL;
}

static zda_inline zda_avl_ht_iter_t zda_avl_ht_get_terminator(zda_avl_ht_t *ht)
{
    zda_avl_ht_iter_t iter;
    iter.ht   = ht;
    iter.node = NULL;
    return iter;
}

ZDA_API zda_avl_ht_iter_t zda_avl_ht_get_first(zda_avl_ht_t *ht) zda_noexcept;

ZDA_API void zda_avl_ht_iter_inc(zda_avl_ht_iter_t *iter) zda_noexcept;

/************************************/
/* Debug APIs */
/************************************/
ZDA_API void zda_avl_ht_print_layout(zda_avl_ht_t *ht, void (*print_cb)(zda_avl_ht_node_t *node))
    zda_noexcept;

/************************************/
/* Wrapper macro */
/************************************/
#define zda_decl_ht_insert_check(func_name, key_type, entry_type)                                  \
    entry_type *func_name(zda_avl_ht_t *ht, key_type key, zda_avl_ht_commit_ctx_t *p_ctx)          \
        zda_noexcept

#define zda_def_avl_ht_insert_check(func_name, key_type, entry_type, get_key, hash, cmp)           \
    zda_decl_ht_insert_check(func_name, key_type, entry_type)                                      \
    {                                                                                              \
        entry_type *p_dup;                                                                         \
        zda_avl_ht_insert_check_inplace(ht, key, entry_type, get_key, hash, cmp, *p_ctx, p_dup);   \
        return p_dup;                                                                              \
    }

#define zda_decl_avl_ht_insert_entry(func_name, entry_type)                                        \
    entry_type *func_name(zda_avl_ht_t *ht, entry_type *entry)

#define zda_def_avl_ht_insert_entry(func_name, entry_type, get_key, hash, cmp)                     \
    zda_decl_avl_ht_insert_entry(func_name, entry_type)                                            \
    {                                                                                              \
        entry_type *p_dup;                                                                         \
        zda_avl_ht_insert_entry_inplace(ht, entry, entry_type, get_key, hash, cmp, p_dup);         \
        return p_dup;                                                                              \
    }

#define zda_decl_ht_search(func_name, key_type, entry_type)                                        \
    entry_type *func_name(zda_avl_ht_t *ht, key_type key) zda_noexcept

#define zda_def_avl_ht_search(func_name, key_type, entry_type, get_key, hash, cmp)                 \
    zda_decl_ht_search(func_name, key_type, entry_type)                                            \
    {                                                                                              \
        entry_type *result;                                                                        \
        zda_avl_ht_search_inplace(ht, key, entry_type, get_key, hash, cmp, result);                \
        return result;                                                                             \
    }

#define zda_decl_ht_remove(func_name, key_type, entry_type)                                        \
    entry_type *func_name(zda_avl_ht_t *ht, key_type key) zda_noexcept

#define zda_def_avl_ht_remove(func_name, key_type, entry_type, get_key, hash, cmp)                 \
    zda_decl_ht_remove(func_name, key_type, entry_type)                                            \
    {                                                                                              \
        entry_type *result;                                                                        \
        zda_avl_ht_remove_inplace(ht, key, entry_type, get_key, hash, cmp, result);                \
        return result;                                                                             \
    }

#define zda_decl_ht_destroy(func_name) void func_name(zda_avl_ht_t *ht)

#define zda_def_avl_ht_destroy(func_name, entry_type, free_cb)                                     \
    void func_name(zda_avl_ht_t *ht)                                                               \
    {                                                                                              \
        zda_avl_ht_destroy_inplace(ht, entry_type, free_cb);                                       \
    }

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif /* guard */
