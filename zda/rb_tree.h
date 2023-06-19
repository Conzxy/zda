// SPDX-LICENSE-IDENTIFIER: MIT

/* Conzxy 2023 6.14
 * This file implements an intrusive red-black tree.
 */
#ifndef _ZDA_RB_TREE_H__
#define _ZDA_RB_TREE_H__

#include <assert.h>

#include "zda/util/export.h"
#include "zda/util/macro.h"
#include "zda/util/container_of.h"

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

typedef enum _zda_color {
  ZDA_RB_COLOR_RED   = 0,
  ZDA_RB_COLOR_BLACK = 1,
} _zda_rb_color_e;

/**
 * This is a intrusive red-black tree node
 * You should embed it to your data structure.
 * e.g.
 * ```C
 * struct user_data {
 *   zda_rb_node *node;
 * };
 * ```
 * If you want use the APIs or Macros provided in this file.
 * you should name the member to `node`.
 * Otherwise, I don't restrict it.
 * You can implement these APIs(search, insert, remove) by
 * `zda_rb_node_entry_custom()`.
 *
 * ```info
 * The Linux rbtree use the lowest two bit to store the color information
 * since it align to sizeof(long).
 * But the long is not cross-platform, in Windows 64bits, sizeof(long) == 4,
 * in Linux 64bits, sizeof(long) == 8.
 *
 * Here, I use a enum color to represent it.
 * ```
 */
typedef struct zda_rb_node {
  /* Private data */
  struct zda_rb_node *left;
  struct zda_rb_node *right;
  struct zda_rb_node *parent;
  _zda_rb_color_e     color;
} zda_rb_node_t;

/* User should use `node` to get the entry struct and access its member
 * to implement the specific logic of entry. */
typedef int (*zda_rb_cmp_t)(zda_rb_node_t const *node, void const *key);
typedef void (*zda_rb_free_t)(zda_rb_node_t *node);
typedef void (*zda_rb_entry_node_op_t)(zda_rb_node_t *node);
typedef void *(*zda_rb_malloc_t)(size_t entry_size);

/**
 * @brief The header sentinel of red-black tree
 * These usages:
 * 1. As the black nil link to avoid some edge condition check in remove and insert(and rebalance)
 * 2. Fetch the first(minimum) and last(maximum) node more fast(time: O(1))
 * 3. Let user use `zda_rb_header header` instead of `zda_rb_node_t *root` to present the tree.
 *
 *                  |+++++++++|
 *                  | header  |
 *                  |+++++++++|
 *               /      | |       \
 * |++++++++++|     |+++++++++|   |++++++++++|
 * | min node |     |  root   |   | max node |
 * |++++++++++|     |+++++++++|   |++++++++++|
 *
 * header->left => node with min entry
 * header->right => node with max entry
 * header->parent => root
 * header->parent->parent => header(ie. root->parent)
 *
 * The left and right child leaf must pointer to the header.
 */
typedef struct zda_rb_header {
  zda_rb_node_t node;
} zda_rb_header_t;

#define ZDA_RB_HEADER_TO_NODE(header) ((zda_rb_node_t *)header)
#define ZDA_RB_HEADER_NODE            ((zda_rb_node_t *)header)

/**
 * @brief Make your custom macro to get entry from node pointer
 * If the member whose type is `zda_rb_header` is not named by `node`,
 * user can custom the macro to get the entry.
 * e.g.
 * ```c
 * typedef struct entry {
 *   zda_rb_node_t *rb_node;
 * } entry_t;
 *
 * #define rb_node_entry(p_node, type) zda_rb_node_entry_custom(p_node, type, rb_node)
 * ```
 */
#define zda_rb_node_entry_custom(p_node, type, node) container_of(p_node, type, node)

/* Like `zda_rb_node_entry_custom` but the member name is specified by `node` */
#define zda_rb_node_entry(p_node, type) container_of(p_node, type, node)

/******************************/
/* Link APIs                  */
/******************************/
/**
 * @brief Check whether the \p node is a header(ie. nil link)
 * @return
 *  - 1: Yes, it is a nil link
 *  - 0: No, it is a actual node store the entry
 */
static zda_inline int zda_rb_node_is_nil(zda_rb_header_t *header, zda_rb_node_t *node)
{
  return &header->node == node;
}
#define _zda_rb_node_set_nil(header, node_) node_ = &(header)->node

/*********************************/
/* Node APIs                     */
/*********************************/
/**
 * @brief Get the root of tree
 */
static zda_inline zda_rb_node_t *zda_rb_tree_get_root(zda_rb_header_t *header)
{
  return header->node.parent;
}

/**
 * @brief Get the address of root of tree
 */
static zda_inline zda_rb_node_t **zda_rb_tree_get_p_root(zda_rb_header_t *header)
{
  return &header->node.parent;
}

/**
 * @brief Initialize the header of tree(ie. sentinel node)
 * @param header Must be uninitialized
 */
ZDA_API void zda_rb_header_init(zda_rb_header_t *header);

/**
 * @brief Initialize the node of tree
 *
 * @param header Must be initialized
 * @param node Must not be uninitialized
 */
ZDA_API void zda_rb_node_init(zda_rb_header_t *header, zda_rb_node_t *node);

/**
 * @brief Check whether the tree has no entry
 *
 * @param header
 * @return
 *  1: YES, it is empty tree.
 */
static zda_inline int zda_rb_tree_is_empty(zda_rb_header_t *header)
{
  return header->node.parent == &header->node;
}

/******************************************************/
/* Iterator APIs                                      */
/******************************************************/
/* Internal usage */
ZDA_NO_API zda_rb_node_t *zda_rb_node_get_min_entry(zda_rb_header_t *header, zda_rb_node_t *root);
ZDA_NO_API zda_rb_node_t *zda_rb_node_get_max_entry(zda_rb_header_t *header, zda_rb_node_t *root);

/**
 * @brief Get the next node whose entry is just greater than \p node
 * @param header
 * @param node node for comparison(ie. the base node)
 * @return
 *  - header: no successor can be found.
 *  - not header: successor node
 *  To check the result can by calling `zda_rb_node_is_nil()`.
 */
ZDA_API zda_rb_node_t *zda_rb_node_get_successor(zda_rb_header_t *header, zda_rb_node_t *node);

/**
 * @brief Get the next node whose entry is just less than \p node
 * @param header
 * @param node
 * @return
 *  Like `zda_rb_node_get_successor()`
 */
ZDA_API zda_rb_node_t *zda_rb_node_get_predecessor(zda_rb_header_t *header, zda_rb_node_t *node);

/**
 * @brief Get the node whose key of entry is the minimum of tree
 * @param header
 * @return
 *  Like `zda_rb_node_get_successor()`
 */
static zda_inline zda_rb_node_t *zda_rb_tree_first(zda_rb_header_t *header)
{
  return header->node.left;
}

/**
 * @brief Like `zda_rb_tree_get_min_entry()`
 * @param header
 * @return
 *  Like `zda_rb_node_get_successor()`
 */
static zda_inline zda_rb_node_t *zda_rb_tree_get_min_entry(zda_rb_header_t *header)
{
  return header->node.left;
}

/**
 * @brief Get the node whose key of entry is the maximum of tree
 *
 * @param header
 * @return
 *  Like `zda_rb_node_get_successor()`
 */
static zda_inline zda_rb_node_t *zda_rb_tree_last(zda_rb_header_t *header)
{
  return header->node.right;
}

/**
 * @brief Like `zda_rb_tree_last()`
 * @param header
 * @return
 *  Like `zda_rb_node_get_successor()`
 */
static zda_inline zda_rb_node_t *zda_rb_tree_get_max_entry(zda_rb_header_t *header)
{
  return header->node.right;
}

/**
 * @brief Get the terminator of iteartor
 * @param header
 * @return
 *  - header: as the terminator of iteration
 */
static zda_inline zda_rb_node_t *zda_rb_tree_terminator(zda_rb_header_t *header)
{
  return &header->node;
}

#define zda_rb_tree_iterate(header)                                                                \
  for (zda_rb_node_t *pos = zda_rb_tree_first(header); pos != zda_rb_tree_terminator(header);      \
       pos                = zda_rb_node_get_successor(header, pos))

/*******************************************/
/* Destroy APIs                            */
/*******************************************/

/**
 * @brief Reuse all nodes in the tree(the nodes will be removed)
 * @param entry_node_cb The callback that do something to node contains entry
 */
ZDA_API void zda_rb_tree_entry_reuse(zda_rb_header_t *header, zda_rb_entry_node_op_t entry_node_cb);

/**
 * @brief Destroy the all nodes of tree
 * @param header
 * @param free_cb The callback that free the entry
 */
static zda_inline void zda_rb_tree_destroy(zda_rb_header_t *header, zda_rb_free_t free_cb)
{
  zda_rb_tree_entry_reuse(header, free_cb);
}

/**
 * @brief Destroy the tree and init the header
 *
 * Init the header to reuse the old tree
 * @param header
 * @param free_cb
 */
static zda_inline void zda_rb_tree_destroy_init(zda_rb_header_t *header, zda_rb_free_t free_cb)
{
  zda_rb_tree_destroy(header, free_cb);
  zda_rb_header_init(header);
}

/***************************************/
/* Search APIs */
/***************************************/
/**
 * @brief Search the entry contains key
 * @return
 *  header - entry node is not found.
 * @note
 *  Don't return NULL to indicates failure,
 *  this is not consistent with some APIs return header.
 */
ZDA_API zda_rb_node_t *zda_rb_tree_search(
    zda_rb_header_t *header,
    void const      *key,
    zda_rb_cmp_t     cmp
);

#define zda_decl_rb_tree_search(func_name, key_type, type)                                         \
  type *func_name(zda_rb_header_t *header, key_type const *key, zda_rb_cmp_t cmp)

#define zda_def_rb_tree_search(func_name, key_type, type)                                          \
  zda_decl_rb_tree_search(func_name, key_type, type)                                               \
  {                                                                                                \
    zda_rb_node_t *node = zda_rb_tree_search(header, key, cmp);                                    \
    return (zda_rb_node_is_nil(header, node)) ? ZDA_NULL : zda_rb_node_entry(node, type);          \
  }

/************************************/
/* Insert APIs */
/************************************/
/**
 * @brief Find the empty slot should place the new entry
 * @param header
 * @param key key of the entry
 * @param cmp Compare callback
 * @param p_slot Store the slot address
 * @param parent The parent of emptry slot(Through the parent member of slot don't get it)
 * @return
 *  1: Succuss, the `p_slot` store the empty slot address and the `parent` store the parent of it.
 *  0: Failure, the `p_slot` store the existed node address and the `parent` is invalid.
 * @note
 *  Usage see `zda_def_rb_tree_insert_malloc()`
 */
ZDA_API int zda_rb_tree_find_insert_slot(
    zda_rb_header_t *header,
    void const      *key,
    zda_rb_cmp_t     cmp,
    zda_rb_node_t ***p_slot,
    zda_rb_node_t  **parent
);

ZDA_API void zda_rb_tree_find_insert_slot_eq(
    zda_rb_header_t *header,
    void const      *key,
    zda_rb_cmp_t     cmp,
    zda_rb_node_t ***p_slot,
    zda_rb_node_t  **parent
);

ZDA_API void zda_rb_node_link(zda_rb_header_t *header, zda_rb_node_t *node, zda_rb_node_t *parent);

ZDA_API void zda_rb_node_insert_rebalance(
    zda_rb_header_t *header,
    zda_rb_node_t   *node,
    zda_rb_node_t   *parent
);

/**
 * @brief Link the node and parent, then rebalance if it is neccessary
 * @param header
 * @param node The new node
 * @param parent The parent of the new node(ie. empty slot)
 * @note
 *  Usage see `zda_def_rb_tree_insert_malloc()`
 */
ZDA_API void zda_rb_node_after_insert(
    zda_rb_header_t *header,
    zda_rb_node_t   *node,
    zda_rb_node_t   *parent
);

/**
 * @brief Declare the rb tree insert prototype of specific key type and entry type
 */
#define zda_decl_rb_tree_insert(func_name, key_type, type)                                         \
  int func_name(zda_rb_header_t *header, key_type const *key, zda_rb_cmp_t cmp, type **p_dup)

/**
 * @brief Define the rb tree insert function of specfiic key type and entry type
 * @note If you want define this to `static` or `inline`, you can write it as following:
 *       `static inline zda_def_rt_tree_insert_malloc(...)`
 */
#define zda_def_rb_tree_insert_malloc(func_name, key_type, type, malloc_cb)                        \
  zda_decl_rb_tree_insert(func_name, key_type, type)                                               \
  {                                                                                                \
    zda_rb_node_t **p_slot    = ZDA_NULL;                                                          \
    zda_rb_node_t  *parent    = ZDA_NULL;                                                          \
    int             insert_ok = zda_rb_tree_find_insert_slot(header, key, cmp, &p_slot, &parent);  \
    if (insert_ok) {                                                                               \
      *p_dup  = (type *)malloc_cb(sizeof(type));                                                   \
      *p_slot = &(*p_dup)->node;                                                                   \
      zda_rb_node_after_insert(header, *p_slot, parent);                                           \
      return 1;                                                                                    \
    }                                                                                              \
    *p_dup = zda_rb_node_entry(*p_slot, type);                                                     \
    return 0;                                                                                      \
  }

/**
 * @brief The malloc callback is specified to malloc() in libc
 */
#define zda_def_rb_tree_insert(func_name, key_type, type)                                          \
  zda_def_rb_tree_insert_malloc(func_name, key_type, type, malloc)

/**********************************/
/* Remove APIs */
/**********************************/
/**
 * @brief Remove the node from tree
 * @param old_node The node must be valid, eg. the node returned from `zda_rb_tree_search()`
 * @warning
 *  This function don't check whether the node is a valid node in the tree.
 */
ZDA_API void zda_rb_tree_remove_node(zda_rb_header_t *header, zda_rb_node_t *old_node);

/**
 * @brief Remove the entry node whose contains \p key
 * @param header
 * @param key
 * @param cmp Compare callback
 * @return
 *  header, no such entry node contains key,
 *  otherwise the removed node.
 * @note
 *  Get the removed node, you can free it or reuse it.
 */
ZDA_API zda_rb_node_t *zda_rb_tree_remove(
    zda_rb_header_t *header,
    void const      *key,
    zda_rb_cmp_t     cmp
);

#define zda_decl_rb_tree_remove(func_name, key_type, type)                                         \
  type *func_name(zda_rb_header_t *header, key_type const *key, zda_rb_cmp_t cmp)

#define zda_def_rb_tree_remove(func_name, key_type, type)                                          \
  zda_decl_rb_tree_remove(func_name, key_type, type)                                               \
  {                                                                                                \
    zda_rb_node_t *old_node = zda_rb_tree_remove(header, key, cmp);                                \
    return (zda_rb_node_is_nil(header, old_node)) ? ZDA_NULL : zda_rb_node_entry(old_node, type);  \
  }

/*********************************************/
/* Debug APIs */
/*********************************************/
ZDA_API size_t zda_rb_tree_get_height(zda_rb_header_t *header, zda_rb_node_t *root);
ZDA_API size_t zda_rb_tree_get_234_height(zda_rb_header_t *header, zda_rb_node_t *root);
#define zda_rb_tree_get_black_height zda_rb_tree_get_234_height

ZDA_API int zda_rb_tree_verify_properties(zda_rb_header_t *header);

/**
 * @brief print the tree as tree structure like this:
 * 95
 * ├── 97
 * |    ├── 98
 * |    |    └── 99
 * |    *── 96
 * *── 93
 *      └── 94
 * @param header
 * @param print_cb
 */
ZDA_API void zda_rb_tree_print_tree(zda_rb_header_t *header, int(print_cb)(void const *node));

/********************************************/
/* Inplace version macro */
/********************************************/

#define zda_rb_tree_destroy_inplace(header, type, free_cb)                                         \
  do {                                                                                             \
    zda_rb_node_t *root = zda_rb_tree_get_root(header);                                            \
    while (!zda_rb_node_is_nil(header, root)) {                                                    \
      if (!zda_rb_node_is_nil(header, root->left)) {                                               \
        root = root->left;                                                                         \
      } else if (!zda_rb_node_is_nil(header, root->right)) {                                       \
        root = root->right;                                                                        \
      } else {                                                                                     \
        zda_rb_node_t *parent = root->parent;                                                      \
        if (parent->left == root) {                                                                \
          _zda_rb_node_set_nil(header, parent->left);                                              \
        } else if (parent->right == root) {                                                        \
          _zda_rb_node_set_nil(header, parent->right);                                             \
        }                                                                                          \
        /* User can reinit the header and here don't reset the header member */                    \
        /* else {                                                                                  \
          (header)->node.left = (header)->node.right = &(header)->node;                            \
        } */                                                                                       \
        free_cb(zda_rb_node_entry(root, type));                                                    \
        root = parent;                                                                             \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#define zda_rb_tree_search_inplace(header, key, type, cmp_cb, p_result)                            \
  do {                                                                                             \
    p_result            = ZDA_NULL;                                                                \
    zda_rb_node_t *root = zda_rb_tree_get_root(header);                                            \
    while (!zda_rb_node_is_nil(header, root)) {                                                    \
      int res = cmp_cb(zda_rb_node_entry(root, type), key);                                        \
      if (res < 0) {                                                                               \
        root = root->right;                                                                        \
      } else if (res > 0) {                                                                        \
        root = root->left;                                                                         \
      } else {                                                                                     \
        p_result = zda_rb_node_entry(root, type);                                                  \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#define zda_rb_tree_insert_inplace(header, key, type, cmp_cb, result, p_dup)                       \
  do {                                                                                             \
    zda_rb_node_t **p_slot       = zda_rb_tree_get_p_root(header);                                 \
    zda_rb_node_t  *track_parent = (*p_slot)->parent;                                              \
    for (; !zda_rb_node_is_nil(header, *p_slot);) {                                                \
      int res = cmp_cb(zda_rb_node_entry(*p_slot, type), key);                                     \
      if (res < 0) {                                                                               \
        track_parent = *p_slot;                                                                    \
        p_slot       = &(*p_slot)->right;                                                          \
      } else if (res > 0) {                                                                        \
        track_parent = *p_slot;                                                                    \
        p_slot       = &(*p_slot)->left;                                                           \
      } else {                                                                                     \
        result = 0;                                                                                \
        p_dup  = zda_rb_node_entry(*p_slot, type);                                                 \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    p_dup = (type *)malloc(sizeof(type));                                                          \
    if (!p_dup) {                                                                                  \
      result = false;                                                                              \
      break;                                                                                       \
    }                                                                                              \
    *p_slot = &p_dup->node;                                                                        \
    zda_rb_node_after_insert(header, &p_dup->node, track_parent);                                  \
    result = 1;                                                                                    \
  } while (0)

#define zda_rb_tree_remove_inplace(header, key, type, cmp_cb, p_entry)                             \
  do {                                                                                             \
    zda_rb_tree_search_inplace(header, key, type, cmp_cb, p_entry);                                \
    if (p_entry) {                                                                                 \
      zda_rb_tree_remove_node(header, &p_entry->node);                                             \
    }                                                                                              \
  } while (0)

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif /* Header guard */
