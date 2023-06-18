// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_LIST_H__
#define _ZDA_LIST_H__

#include "zda/util/macro.h"
#include "zda/util/container_of.h"

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

typedef struct zda_list_node {
  struct zda_list_node *next;
  struct zda_list_node *prev;
} zda_list_node_t;

typedef struct zda_list_node zda_list_header;
typedef zda_list_node_t      zda_list_header_t;

#define zda_list_node_entry(p_node, type) container_of(p_node, type, node)
#define zda_list_get_entry(type)          zda_list_node_entry(pos, type)

/* `static inline` is necessary for ISO C89/C90.
 * In C89, The compiler will think the non-static inline function is a usual function */
static zda_inline void zda_list_node_sentinel_init(zda_list_node_t *sentinel)
{
  sentinel->prev = sentinel;
  sentinel->next = sentinel;
}

#define zda_list_node_header_init zda_list_node_sentinel_init

/******************************************/
/* Insert APIs                            */
/******************************************/

static zda_inline void zda_list_node_insert_before(zda_list_node_t *pos, zda_list_node_t *new_node)
{
  new_node->next  = pos;
  new_node->prev  = pos->prev;
  pos->prev->next = new_node;
  pos->prev       = new_node;
}
static zda_inline void zda_list_node_insert_after(zda_list_node_t *pos, zda_list_node_t *new_node)
{
  new_node->next  = pos->next;
  new_node->prev  = pos;
  pos->next->prev = new_node;
  pos->next       = new_node;
}

#define zda_list_insert_after zda_list_node_insert_after

static zda_inline void zda_list_push_front(zda_list_header_t *header, zda_list_node_t *new_node)
{
  zda_list_node_insert_after(header, new_node);
}

static zda_inline void zda_list_push_back(zda_list_header_t *header, zda_list_node_t *new_node)
{
  zda_list_node_insert_before(header, new_node);
}

/*******************************************/
/* Remove APIs                             */
/*******************************************/

static zda_inline void zda_list_node_remove(zda_list_node_t *old_node)
{
  old_node->next->prev = old_node->prev;
  old_node->prev->next = old_node->next;
}

#define zda_list_remove zda_list_node_remove

#define zda_list_destroy(header, type, free_cb)                                                    \
  do {                                                                                             \
    zda_list_iterate(header)                                                                       \
    {                                                                                              \
      free_cb(container_of(pos, type, node));                                                      \
    }                                                                                              \
    (header)->prev = (header)->next = ZDA_NULL;                                                    \
  } while (0)

static zda_inline void zda_list_pop_front(zda_list_header_t *header, zda_list_node_t *old_node)
{
  zda_list_node_remove(header->next);
}

static zda_inline void zda_list_pop_back(zda_list_node_t *header, zda_list_node_t *old_node)
{
  zda_list_node_remove(header->prev);
}

/***********************************/
/* Search APIs                     */
/***********************************/

#define zda_list_iterate(header)                                                                   \
  for (zda_list_node_t *pos = (header)->next; pos != (header); pos = pos->next)

#define zda_list_search(header, type, member, val, p_entry)                                        \
  do {                                                                                             \
    zda_list_iterate(header)                                                                       \
    {                                                                                              \
      type *entry = zda_list_node_entry(pos, type);                                                \
      if (entry->member == val) {                                                                  \
        p_entry = entry;                                                                           \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif