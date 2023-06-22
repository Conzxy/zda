// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_LIST_H__
#define _ZDA_LIST_H__

#include "zda/rb_tree.h"
#include "zda/util/macro.h"
#include "zda/util/container_of.h"
#include "zda/util/bool.h"

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

/**
 * @brief The node store the user data(for user side)
 */
typedef struct zda_slist_node {
  struct zda_slist_node *next;
} zda_slist_node_t;

/**
 * @brief Single linked-list with a sentinel header that don't store data
 */
typedef struct zda_slist_header {
  zda_slist_node_t node;
} zda_slist_header_t;

/**
 * @brief Single linked-list with a heaeder that stores user data
 *
 */
typedef struct zda_slist_header2 {
  zda_slist_node_t *node;
} zda_slist_header2_t;

typedef zda_slist_header_t  zda_slist_t;
typedef zda_slist_header2_t zda_slist2_t;

typedef int (*zda_slist_cmp_t)(zda_slist_node_t const *node, void const *key);

#define zda_slist_node_entry(p_node, type, member) container_of(p_node, type, member)
#define zda_slist_entry(p_node, type)              zda_slist_node_entry(p_node, type, node)

static zda_inline void zda_slist_sentinel_init(zda_slist_header_t *header)
{
  header->node.next = NULL;
}

#define zda_slist_header_init zda_slist_sentinel_init

static zda_inline int zda_slist_is_empty(zda_slist_header_t *header)
{
  return header->node.next == NULL;
}

static zda_inline zda_slist_node_t *zda_slist_front(zda_slist_header_t *header)
{
  return header->node.next;
}

static zda_inline void zda_slist_insert_after(zda_slist_node_t *pos, zda_slist_node_t *new_node)
{
  assert(pos);
  new_node->next = pos->next;
  pos->next      = new_node;
}

static zda_inline void zda_slist_push_front(zda_slist_header_t *header, zda_slist_node_t *new_node)
{
  zda_slist_insert_after(&header->node, new_node);
}

static zda_inline zda_slist_node_t *zda_slist_remove_after(zda_slist_node_t *pos)
{
  zda_slist_node_t *ret = pos->next;
  pos->next             = ret->next;
  return ret;
}

static zda_inline zda_slist_node_t *zda_slist_pop_front(zda_slist_header_t *header)
{
  return zda_slist_remove_after(&header->node);
}

#define zda_slist_iterate(header)                                                                  \
  for (zda_slist_node_t *pos = (header)->node.next; pos != NULL; pos = pos->next)

/**
 * @brief Search entry that satisfy the \p cmp
 * I don't encourage user to use this function since \p cmp
 * maybe don't inlined(ie. expane inplace).
 * Instead, user can use the `zda_slist_search_inplace()`
 * or wrap it to a actual function.
 * @param header
 * @param key
 * @param cmp
 * @return
 *  - NULL: The entry does not exists
 */
ZDA_API zda_slist_node_t *zda_slist_search(
    zda_slist_header_t *header,
    void const         *key,
    zda_slist_cmp_t     cmp
);

#define zda_slist_search_inplace(header, val, type, member, cmp, p_entry)                          \
  do {                                                                                             \
    zda_slist_iterate(header)                                                                      \
    {                                                                                              \
      type *entry = zda_slist_entry(pos, type);                                                    \
      if (cmp(entry->member, val)) {                                                               \
        p_entry = entry;                                                                           \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

/*******************************/
/* No sentinel version APIs */
/*******************************/
static zda_inline void zda_slist_header_init2(zda_slist_header2_t *header) { header->node = NULL; }

static zda_inline zda_bool zda_slist_is_empty2(zda_slist_header2_t *header)
{
  return header->node == NULL;
}

static zda_inline void zda_slist_push_front2(zda_slist_header2_t *header, zda_slist_node_t *node)
{
  node->next   = header->node;
  header->node = node;
}

static zda_inline zda_slist_node_t *zda_slist_front2(zda_slist_header2_t *header)
{
  return header->node;
}

static zda_inline void zda_slist_pop_front2(zda_slist_header2_t *header)
{
  assert(header->node);
  header->node = header->node->next;
}

#define zda_slist_iterate2(header)                                                                 \
  for (zda_slist_node_t *pos = (header)->node; pos != NULL; pos = pos->next)

ZDA_API zda_slist_node_t *zda_slist_search2(
    zda_slist_header2_t *header,
    void const          *key,
    zda_slist_cmp_t      cmp
);

#define zda_list_search2_inplace(header, key, member, cmp, p_entry)                                \
  do {                                                                                             \
    zda_slist_iterate2(header)                                                                     \
    {                                                                                              \
      type *entry = zda_slist_entry(pos, type);                                                    \
      if (cmp(entry->member, key)) {                                                               \
        p_entry = entry;                                                                           \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  }

#define zda_slist_destroy_free(header, type, free_cb)                                              \
  do {                                                                                             \
    for (zda_slist_node_t *pos = (header)->node.next; pos != NULL;) {                              \
      zda_slist_node_t *old_next = pos->next;                                                      \
      free_cb(zda_slist_entry(pos, type));                                                         \
      pos = old_next;                                                                              \
    }                                                                                              \
  } while (0)

#define zda_slist_destroy_free2(header, type, free_cb)                                             \
  do {                                                                                             \
    for (zda_slist_node_t *pos = (header)->node; pos != NULL;) {                                   \
      {                                                                                            \
        zda_slist_node_t *old_next = pos->next;                                                    \
        free_cb(zda_slist_entry(pos, type));                                                       \
        pos = old_next;                                                                            \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#define zda_slist_destroy(header, type) zda_slist_destroy_free(header, type, free)

#define zda_slist_destroy2(header, type) zda_slist_destroy_free2(header, type, free)

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif /* Header guard */