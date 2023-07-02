// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_AVL_TREE_H__
#define _ZDA_AVL_TREE_H__

/*
 * 2023/6/26 Conzxy
 */
#include "zda/util/macro.h"
#include "zda/util/bool.h"
#include "zda/util/export.h"
#include "zda/util/container_of.h"
#include "zda/util/assert.h"
#include <stddef.h>

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

/**
 * @brief Represent a avl-tree node
 * The node stores the height instead compute height when
 * it is used.
 */
typedef struct zda_avl_node {
  struct zda_avl_node *left;
  struct zda_avl_node *right;
  struct zda_avl_node *parent;
  size_t               height;
} zda_avl_node_t;

#define ZDA_AVL_NODE_HOOK zda_avl_node_t node

#define zda_avl_node_entry(p_node, type, node) container_of(p_node, type, node)

/* Like `zda_avl_node_entry` but the member name is specified by `node` */
#define zda_avl_entry(p_node, type) container_of(p_node, type, node)

/**
 * @brief Represents a avl tree
 * This is not a sentinel node.
 * To avl-tree, the sentinel is not efficient compared to rb-tree
 */
typedef struct zda_avl_tree {
  zda_avl_node_t *node;
} zda_avl_tree_t;

/*******************************/
/* Initializer */
/*******************************/
static zda_inline void zda_avl_tree_init(zda_avl_tree_t *tree) zda_noexcept { tree->node = NULL; }

static zda_inline void zda_avl_node_init(zda_avl_node_t *node) zda_noexcept
{
  node->height = 0;
  node->left = node->right = node->parent = NULL;
}

/******************************/
/* Getter */
/******************************/
static zda_inline zda_bool zda_avl_node_is_single(zda_avl_node_t *node) zda_noexcept
{
  return node && (!node->left && !node->right);
}

static zda_inline size_t zda_avl_tree_is_single(zda_avl_tree_t *tree) zda_noexcept
{
  return zda_avl_node_is_single(tree->node);
}

static zda_inline zda_avl_node_t *zda_avl_tree_get_root(zda_avl_tree_t *tree) zda_noexcept
{
  return tree->node;
}

static zda_inline zda_bool zda_avl_tree_is_empty(zda_avl_tree_t *tree) zda_noexcept
{
  return tree->node == NULL;
}

/***************************/
/* Iterator APIs */
/***************************/
static zda_inline zda_avl_node_t *zda_avl_node_get_min(zda_avl_node_t *node) zda_noexcept
{
  zda_assert0(node);
  while (node->left) {
    node = node->left;
  }
  return node;
}

static zda_inline zda_avl_node_t *zda_avl_node_get_max(zda_avl_node_t *node) zda_noexcept
{
  zda_assert0(node);
  while (node->right)
    node = node->right;
  return node;
}

static zda_inline zda_avl_node_t *zda_avl_tree_get_first(zda_avl_tree_t *tree) zda_noexcept
{
  if (!tree->node) {
    return NULL;
  }
  return zda_avl_node_get_min(tree->node);
}

static zda_inline zda_avl_node_t *zda_avl_tree_get_last(zda_avl_tree_t *tree) zda_noexcept
{
  if (!tree->node) {
    return tree->node;
  }
  return zda_avl_node_get_max(tree->node);
}

static zda_inline zda_avl_node_t *zda_avl_tree_get_terminator(zda_avl_tree_t *tree)
{
  (void)tree;
  return NULL;
}

static zda_inline zda_avl_node_t *zda_avl_node_get_next(zda_avl_node_t *node)
{
  if (node->right) {
    return zda_avl_node_get_min(node->right);
  }
  zda_avl_node_t *parent = node->parent;
  while (parent && parent->right == node) {
    node   = parent;
    parent = node->parent;
  }
  return parent;
}

static zda_inline zda_avl_node_t *zda_avl_node_get_prev(zda_avl_node_t *node)
{
  if (node->left) {
    return zda_avl_node_get_max(node->left);
  }
  zda_avl_node_t *parent = node->parent;
  while (parent && parent->left == node) {
    node   = parent;
    parent = node->parent;
  }
  return parent;
}

/**************************/
/* Insert APIs */
/**************************/

/**
 * @brief Store the context after insert check
 * Because the insert check function don't known the new node,
 * the context must record the empty slot and its parent.
 * (slot->parent can't get its parent since slot is null link)
 */
typedef struct zda_avl_commit_ctx {
  zda_avl_node_t **pp_slot;
  zda_avl_node_t  *p_parent;
} zda_avl_commit_ctx_t;

#define zda_avl_tree_insert_check_inplace(tree, key, type, get_key, cmp_cb, commit_ctx, p_dup)     \
  do {                                                                                             \
    zda_avl_tree_t  *__tree       = tree;                                                          \
    zda_avl_node_t **p_slot       = &(__tree->node);                                               \
    zda_avl_node_t  *track_parent = NULL;                                                          \
    p_dup                         = NULL;                                                          \
    for (; *p_slot;) {                                                                             \
      int res = cmp_cb(get_key(zda_avl_entry(*p_slot, type)), key);                                \
      if (res < 0) {                                                                               \
        track_parent = *p_slot;                                                                    \
        p_slot       = &(*p_slot)->right;                                                          \
      } else if (res > 0) {                                                                        \
        track_parent = *p_slot;                                                                    \
        p_slot       = &(*p_slot)->left;                                                           \
      } else {                                                                                     \
        p_dup = zda_avl_entry(*p_slot, type);                                                      \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    if (p_dup) break;                                                                              \
    (commit_ctx).pp_slot  = p_slot;                                                                \
    (commit_ctx).p_parent = track_parent;                                                          \
  } while (0)

#define zda_decl_avl_tree_insert_check(func_name, key_type, type)                                  \
  type *func_name(zda_avl_tree_t *tree, key_type key, zda_avl_commit_ctx_t *p_ctx)

#define zda_def_avl_tree_insert_check(func_name, key_type, type, get_key, cmp)                     \
  zda_decl_avl_tree_insert_check(func_name, key_type, type)                                        \
  {                                                                                                \
    type *p_dup;                                                                                   \
    zda_avl_tree_insert_check_inplace(tree, key, type, get_key, cmp, *p_ctx, p_dup);               \
    return p_dup;                                                                                  \
  }

/* Because it is not necessary that pass compare function
 * and inline it.
 * We can declare this as a function instead of function-like macro.
 */
ZDA_API void zda_avl_tree_insert_commit(
    zda_avl_tree_t       *tree,
    zda_avl_commit_ctx_t *p_ctx,
    zda_avl_node_t       *new_node
) zda_noexcept;

#define zda_avl_tree_insert_entry_inplace(tree, entry, type, get_key, cmp_cb, p_dup)               \
  do {                                                                                             \
    zda_avl_commit_ctx_t cmt_ctx;                                                                  \
    type                *p_dup;                                                                    \
    zda_avl_tree_insert_check_inplace(                                                             \
        tree,                                                                                      \
        get_key(entry),                                                                            \
        type,                                                                                      \
        get_key,                                                                                   \
        cmp_cb,                                                                                    \
        cmt_ctx,                                                                                   \
        p_dup                                                                                      \
    );                                                                                             \
    if (p_dup) break;                                                                              \
    zda_avl_tree_insert_commit(tree, &cmt_ctx, &entry->node);                                      \
  } while (0)

#define zda_decl_avl_tree_insert_entry(func_name, type)                                            \
  type *func_name(zda_avl_tree_t *tree, type *entry)

#define zda_def_avl_tree_insert_entry(func_name, type, get_key, cmp_cb)                            \
  zda_decl_avl_tree_insert_entry(func_name, type)                                                  \
  {                                                                                                \
    type *p_dup;                                                                                   \
    zda_avl_tree_insert_entry_inplace(tree, entry, type, get_key, cmp_cb, p_dup);                  \
    return p_dup;                                                                                  \
  }

/********************************/
/* Search APIs */
/********************************/
#define zda_avl_tree_search_inplace(tree, key, type, get_key, cmp_cb, p_result)                    \
  do {                                                                                             \
    zda_avl_tree_t *__tree = tree;                                                                 \
    p_result               = NULL;                                                                 \
    zda_avl_node_t *root   = __tree->node;                                                         \
    while (root) {                                                                                 \
      int res = cmp_cb(get_key(zda_avl_entry(root, type)), key);                                   \
      if (res < 0) {                                                                               \
        root = root->right;                                                                        \
      } else if (res > 0) {                                                                        \
        root = root->left;                                                                         \
      } else {                                                                                     \
        p_result = zda_avl_entry(root, type);                                                      \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#define zda_decl_avl_tree_search(func_name, key_type, type)                                        \
  type *func_name(zda_avl_tree_t *tree, key_type key)

#define zda_def_avl_tree_search(func_name, key_type, type, get_key, cmp)                           \
  zda_decl_avl_tree_search(func_name, key_type, type)                                              \
  {                                                                                                \
    type *result;                                                                                  \
    zda_avl_tree_search_inplace(tree, key, type, get_key, cmp, result);                            \
    return result;                                                                                 \
  }

/*********************************/
/* Destroy APIs */
/*********************************/
#define zda_avl_tree_destroy_inplace(tree, type, free_cb)                                          \
  do {                                                                                             \
    zda_avl_tree_t *__tree = tree;                                                                 \
    zda_avl_node_t *root   = __tree->node;                                                         \
    while (root) {                                                                                 \
      if (root->left) {                                                                            \
        root = root->left;                                                                         \
      } else if (root->right) {                                                                    \
        root = root->right;                                                                        \
      } else {                                                                                     \
        zda_avl_node_t *parent = root->parent;                                                     \
        if (parent) {                                                                              \
          if (parent->left == root) {                                                              \
            parent->left = NULL;                                                                   \
          } else {                                                                                 \
            assert(parent->right == root);                                                         \
            parent->right = NULL;                                                                  \
          }                                                                                        \
        }                                                                                          \
        /* User can reinit the __tree and here don't reset the tree member */                      \
        /* else {                                                                                  \
          (__tree)->node.left = (tree)->node.right = &(tree)->node;                                \
        } */                                                                                       \
        free_cb(zda_avl_entry(root, type));                                                        \
        root = parent;                                                                             \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#define zda_decl_avl_tree_destroy(func_name) void func_name(zda_avl_tree_t *tree)

#define zda_def_avl_tree_destroy(func_name, type, free_cb)                                         \
  zda_decl_avl_tree_destroy(func_name)                                                             \
  {                                                                                                \
    zda_avl_tree_destroy_inplace(tree, type, free_cb);                                             \
  }

/************************************/
/* Remove APIs */
/************************************/
ZDA_API
void zda_avl_tree_remove_node(zda_avl_tree_t *tree, zda_avl_node_t *old_node) zda_noexcept;

#define zda_avl_tree_remove_inplace(tree, key, type, get_key, cmp_cb, p_entry)                     \
  do {                                                                                             \
    zda_avl_tree_search_inplace(tree, key, type, get_key, cmp_cb, p_entry);                        \
    if (p_entry) {                                                                                 \
      zda_avl_tree_remove_node(tree, &p_entry->node);                                              \
    }                                                                                              \
  } while (0)

#define zda_decl_avl_tree_remove(func_name, key_type, type)                                        \
  type *func_name(zda_avl_tree_t *tree, key_type key)

#define zda_def_avl_tree_remove(func_name, key_type, type, get_key, cmp_cb)                        \
  zda_decl_avl_tree_remove(func_name, key_type, type)                                              \
  {                                                                                                \
    type *ret;                                                                                     \
    zda_avl_tree_remove_inplace(tree, key, type, get_key, cmp_cb, ret);                            \
    return ret;                                                                                    \
  }

/************************************/
/* Debug APIs */
/************************************/
static zda_inline size_t zda_avl_tree_get_height(zda_avl_tree_t *tree) zda_noexcept
{
  return tree->node->height;
}

ZDA_API zda_bool zda_avl_tree_verify_properties(zda_avl_tree_t *tree) zda_noexcept;

ZDA_API void zda_avl_tree_print_tree(
    zda_avl_tree_t *tree,
    int(print_cb)(zda_avl_node_t const *node)
);

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif /*  header guard */
