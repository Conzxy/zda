// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_LIST_H__
#define _ZDA_LIST_H__

#include "zda/util/assert.h"
#include "zda/util/macro.h"
#include "zda/util/container_of.h"
#include "zda/util/bool.h"

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

/**
 * @brief The node that stores the user data
 */
typedef struct zda_list_node {
  struct zda_list_node *next;
  struct zda_list_node *prev;
} zda_list_node_t;

/**
 * @brief Double-linked-list with a sentinel header that don't store data
 */
typedef struct zda_list_header {
  zda_list_node node;
} zda_list_header_t;

/**
 * @brief Double-linked-list with a heaeder that stores user data
 */
typedef struct zda_list_header2 {
  zda_list_node *node;
} zda_list_header2_t;

#define zda_list_node_entry(p_node, type, node) container_of(p_node, type, node)
#define zda_list_entry(pos, type)               zda_list_node_entry(pos, type, node)

/* `static inline` is necessary for ISO C89/C90.
 * In C89, The compiler will think the non-static inline function is a usual function */
static zda_inline void zda_list_node_sentinel_init(zda_list_node_t *sentinel)
{
  sentinel->prev = sentinel;
  sentinel->next = sentinel;
}

static zda_inline void zda_list_header_init(zda_list_header_t *header)
{
  zda_list_node_sentinel_init(&header->node);
}

static zda_inline zda_bool zda_list_is_empty(zda_list_header_t *header)
{
  return header->node.prev == &header->node;
}

static zda_inline zda_list_node_t *zda_list_front(zda_list_header_t *header)
{
  return header->node.next;
}

static zda_inline zda_list_node_t *zda_list_back(zda_list_header_t *header)
{
  return header->node.prev;
}

/******************************************/
/* Insert APIs                            */
/******************************************/

static zda_inline void zda_list_insert_before(zda_list_node_t *pos, zda_list_node_t *new_node)
{
  new_node->next  = pos;
  new_node->prev  = pos->prev;
  pos->prev->next = new_node;
  pos->prev       = new_node;
}
static zda_inline void zda_list_insert_after(zda_list_node_t *pos, zda_list_node_t *new_node)
{
  new_node->next  = pos->next;
  new_node->prev  = pos;
  pos->next->prev = new_node;
  pos->next       = new_node;
}

static zda_inline void zda_list_push_front(zda_list_header_t *header, zda_list_node_t *new_node)
{
  zda_list_insert_after(&header->node, new_node);
}

static zda_inline void zda_list_push_back(zda_list_header_t *header, zda_list_node_t *new_node)
{
  zda_list_insert_before(&header->node, new_node);
}

/*******************************************/
/* Remove APIs                             */
/*******************************************/

static zda_inline void zda_list_remove(zda_list_node_t *old_node)
{
  old_node->next->prev = old_node->prev;
  old_node->prev->next = old_node->next;
}

#define zda_list_destroy(header, type, free_cb)                                                    \
  do {                                                                                             \
    zda_list_header_t *__header = header;                                                          \
    zda_list_node_t   *next;                                                                       \
    for (zda_list_node_t *pos = __header->node.next; pos != &__header->node;) {                    \
      next = pos->next;                                                                            \
      free_cb(zda_list_entry(pos, type));                                                          \
      pos = next;                                                                                  \
    }                                                                                              \
  } while (0)

#define zda_list_destroy_init(header, type, free_cb)                                               \
  do {                                                                                             \
    zda_list_destroy(header, type, free_cb);                                                       \
    zda_list_header_init(header);                                                                  \
  } while (0)

static zda_inline void zda_list_pop_front(zda_list_header_t *header)
{
  zda_list_remove(header->node.next);
}

static zda_inline void zda_list_pop_back(zda_list_header_t *header)
{
  zda_list_remove(header->node.prev);
}

/***********************************/
/* Search APIs                     */
/***********************************/

#define zda_list_iterate(header)                                                                   \
  for (zda_list_node_t *pos = (header)->node.next; pos != &((header)->node); pos = pos->next)

#define zda_list_search(header, val, type, member, cmp, p_entry)                                   \
  do {                                                                                             \
    zda_list_header_t *_header = header;                                                           \
    zda_list_iterate(_header)                                                                      \
    p_entry = NULL;                                                                                \
    zda_list_iterate(_header)                                                                      \
    {                                                                                              \
      type *entry = zda_list_entry(pos, type);                                                     \
      if (cmp(entry->member, val)) {                                                               \
        p_entry = entry;                                                                           \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

/**************************************/
/* No sentinel version */
/**************************************/
static zda_inline void zda_list_header2_init(zda_list_header2_t *header) { header->node = NULL; }

static zda_inline zda_bool zda_list_is_empty2(zda_list_header2_t *header)
{
  return header->node == NULL;
}

static zda_inline void zda_list_push_front2(zda_list_header2_t *header, zda_list_node_t *node)
{
  if (header->node) {
    zda_list_insert_after(header->node, node);
  } else {
    header->node = node;
    node->prev = node->next = node;
  }
}

static zda_inline void zda_list_push_back2(zda_list_header2_t *header, zda_list_node_t *node)
{
  if (header->node) {
    zda_list_insert_before(header->node, node);
  } else {
    header->node = node;
    node->prev = node->next = node;
  }
}

typedef struct zda_list_iter2 {
  zda_list_node_t *node;
  int              pass;
} zda_list_iter2_t;

static zda_inline zda_list_iter2 zda_list_first(zda_list_header2_t *header)
{
  zda_list_iter2 iter;
  iter.node = header->node;
  iter.pass = 0;
  return iter;
}

static zda_inline zda_list_iter2 zda_list_terminator(zda_list_header2_t *header)
{
  zda_list_iter2 iter;
  iter.node = header->node;
  iter.pass = 1;
  return iter;
}

static zda_inline void zda_list_iter_inc2(zda_list_iter2 *iter) { iter->node = iter->node->next; }

static zda_inline zda_bool zda_list_is_terminator2(zda_list_header2_t *header, zda_list_iter2 *iter)
{
  /* if (iter->node == NULL) return zda_true;
  if (iter->node == header->node) {
    if (iter->pass == 0) {
      iter->pass = 1;
      return zda_false;
    }
    return zda_true;
  }
  return zda_false; */
  auto ret = (!iter->node) || (iter->node == header->node && iter->pass > 0);
  if (!ret) iter->pass = 1;
  return ret;
}

static zda_inline zda_list_node_t *zda_list_front2(zda_list_header2_t *header)
{
  return header->node;
}

static zda_inline zda_list_node_t *zda_list_back2(zda_list_header2_t *header)
{
  return header->node->prev;
}

static zda_inline void zda_list_pop_front2(zda_list_header2_t *header)
{
  assert(header->node);
  if (header->node->next) {
    zda_list_remove(header->node);
  } else {
    header->node = NULL;
  }
}

static zda_inline void zda_list_pop_back2(zda_list_header2_t *header)
{
  assert(header->node);
  if (header->node->prev) {
    zda_list_remove(header->node->prev);
  } else {
    header->node = NULL;
  }
}

static zda_inline void zda_list_remove2(zda_list_header2_t *header, zda_list_node_t *node)
{
  assert(node);
  if (node->prev == node) {
    header->node = NULL;
  } else {
    zda_list_remove(node);
  }
}

#define zda_list_search2(header, val, type, member, cmp, p_entry)                                  \
  do {                                                                                             \
    zda_list_header2_t *__header = header;                                                         \
    zda_list_node_t    *first    = __header->node;                                                 \
    p_entry                      = NULL;                                                           \
    for (; first != NULL;) {                                                                       \
      type *cur_entry = zda_list_entry(first, type);                                               \
      if (cmp(cur_entry->member, val)) {                                                           \
        p_entry = cur_entry;                                                                       \
        break;                                                                                     \
      }                                                                                            \
      first = first->next;                                                                         \
      if (first == __header->node) {                                                               \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#define zda_list_destroy2(header, type, free_cb)                                                   \
  do {                                                                                             \
    zda_list_header2_t *__header = header;                                                         \
    zda_list_node_t    *first    = __header->node;                                                 \
    for (; first != NULL;) {                                                                       \
      zda_list_node_t *next = first->next;                                                         \
      free_cb(zda_list_entry(first, type));                                                        \
      first = next;                                                                                \
      if (first == __header->node) {                                                               \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#define zda_list_destroy_init2(header, type, free_cb)                                              \
  do {                                                                                             \
    zda_list_destroy2(header, type, free_cb);                                                      \
    zda_list_header2_init(header);                                                                 \
  } while (0)

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif /* Header guard */