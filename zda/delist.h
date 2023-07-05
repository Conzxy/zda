#ifndef __ZDA_DELIST_H__
#define __ZDA_DELIST_H__

#include "zda/slist.h"

/* Reuse the slsit node */
typedef struct zda_slist_node zda_delist_node;
typedef zda_slist_node_t zda_delist_node_t;
#define zda_delist_node_entry(p_node, type, member) container_of(p_node, type, member)
#define zda_delist_entry(p_node, type)              zda_delist_node_entry(p_node, type, node)

/* Specialized structure supports push back in O(1) time complexity.
 * But notice, this structure don't supports pop back in O(1) time complexity.
 * So the best usage is use this as queue-like container, i.e.,
 * push_back() combine with the pop_front(). 
 * The overhead is a tail pointer in the header sentinel that is very cheap for some scenarios.
 * e.g. Output buffer for network library: consume them used buffer in the front and push 
 * new contents to the back of the list.
 */
typedef struct zda_delist {
  zda_slist_t list;
  zda_delist_node_t *tail; 
} zda_delist_t;

static zda_inline void zda_delist_init(zda_delist_t *dl) zda_noexcept
{
  zda_slist_header_init(&dl->list);
  dl->tail = NULL;
}

static zda_inline zda_bool zda_delist_is_empty(zda_delist_t *dl) zda_noexcept 
{
  return (zda_slist_is_empty(&dl->list));
}

static zda_inline zda_bool zda_delist_is_single(zda_delist_t *dl) zda_noexcept 
{
  return zda_slist_is_single(&dl->list);
}

static zda_inline zda_delist_node_t *zda_delist_front(zda_delist_t *dl) zda_noexcept
{
  return zda_slist_front(&dl->list);
}

static zda_inline zda_delist_node_t *zda_delist_back(zda_delist_t *dl) zda_noexcept
{
  return dl->tail;
}


/****************************/
/* Insert APIs */
/****************************/
static zda_inline void zda_delist_insert_after(zda_delist_t *dl, zda_delist_node_t *pos, zda_delist_node_t *node) zda_noexcept
{
  zda_slist_insert_after(pos, node);
  if (pos == dl->tail) {
    dl->tail = node;
  }
}

static zda_inline void zda_delist_push_front(zda_delist_t *dl, zda_delist_node_t *node) zda_noexcept
{
  zda_delist_insert_after(dl, zda_slist_get_header_node(&dl->list), node);
}

static zda_inline void zda_delist_push_back(zda_delist_t *dl, zda_delist_node_t *node) zda_noexcept
{
  zda_delist_insert_after(dl, dl->tail, node);
}

/*******************************/
/* Remove APIs */
/*******************************/

static zda_inline zda_delist_node_t *zda_delist_remove_after(zda_delist_t *dl, zda_delist_node_t *pos) zda_noexcept
{
  zda_delist_node_t *ret = zda_slist_remove_after(pos);
  /* if (pos == dl->tail_before) { */
  /*   Can't update the tail_before */
  /* } */
  return ret;
}

static zda_inline zda_delist_node_t *zda_delist_pop_front(zda_delist_t *dl) zda_noexcept
{
  if (zda_delist_is_single(dl)) {
    dl->tail = NULL;
  }
  zda_delist_node_t *ret = zda_slist_pop_front(&dl->list);
  return ret;
}

/* The single tail node is not enough to implement the pop_back.
 * Even though record the tail_before_before, the before_before_before is also required in the next pop_back().
 * The only solution is use the list instead delist. */
/* static zda_inline zda_delist_node_t *zda_delist_pop_back(zda_delist_t *dl) zda_noexcept
{
} */

/*****************************/
/* Iterator APIs */
/*****************************/

#define delist_iterate(dl) \
  for (zda_delist_node_t *pos = (header)->node.next; pos != NULL; pos = pos->next)

/*****************************/
/* Search APIs */
/*****************************/

#define zda_delist_search_inplace(header, val, type, member, cmp, p_entry)                          \
  do {                                                                                             \
    zda_delist_iterate(header)                                                                      \
    {                                                                                              \
      type *entry = zda_delist_entry(pos, type);                                                    \
      if (cmp(entry->member, val)) {                                                               \
        p_entry = entry;                                                                           \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#endif /* guard */