// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_LIST_H__
#define _ZDA_LIST_H__

#include "zda/rb_tree.h"
#include "zda/util/macro.h"
#include "zda/util/container_of.h"

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

typedef struct zda_slist_node {
  struct zda_slist_node *next;
} zda_slist_node_t;

typedef struct zda_slist_node zda_slist_header;
typedef zda_slist_node_t      zda_slist_header_t;

#define zda_slist_node_entry(p_node, type) container_of(p_node, type, node)
#define zda_slist_get_entry(type)          zda_slist_node_entry(pos, type)

static zda_inline void zda_slist_sentinel_init(zda_slist_header_t *header)
{
  header->next = ZDA_NULL;
}
#define zda_slist_header_init zda_slist_sentinel_init

static zda_inline void zda_slist_insert_after(zda_slist_node_t *pos, zda_slist_node_t *new_node)
{
  new_node->next = pos->next;
  pos->next      = new_node;
}

static zda_inline void zda_slist_push_front(zda_slist_header_t *header, zda_slist_node_t *new_node)
{
  zda_slist_insert_after(header, new_node);
}

static zda_inline zda_slist_node_t *zda_slist_remove_after(zda_slist_node_t *pos)
{
  zda_slist_node_t *ret = pos->next;
  pos->next             = ret->next;
  return ret;
}

static zda_inline zda_slist_node_t *zda_slist_pop_front(zda_slist_header_t *header)
{
  return zda_slist_remove_after(header);
}

#define zda_slist_iterate(header)                                                                  \
  for (zda_slist_node_t *pos = (header)->next; pos != (header); pos = pos->next)

#define zda_slist_search(header, val, type, member)                                                \
  do {                                                                                             \
    zda_slist_iterate(header)                                                                      \
    {                                                                                              \
      type *entry = zda_slist_node_entry(pos, type);                                               \
      if (entry->member == val) {                                                                  \
        p_entry = entry;                                                                           \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif /* Header guard */