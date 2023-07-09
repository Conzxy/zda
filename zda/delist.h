#ifndef __ZDA_DELIST_H__
#define __ZDA_DELIST_H__

#include "zda/slist.h"
#include "zda/util/swap.h"

/* Reuse the slsit node */
typedef struct zda_slist_node zda_delist_node;
typedef zda_slist_node_t      zda_delist_node_t;
#define zda_delist_node_entry(p_node, type, member) container_of(p_node, type, member)
#define zda_delist_entry(p_node, type)              zda_delist_node_entry(p_node, type, node)

#define ZDA_DELIST_HOOK zda_delist_node_t node

/* Specialized structure supports push back in O(1) time complexity.
 * But notice, this structure don't supports pop back in O(1) time complexity.
 * So the best usage is use this as queue-like container, i.e.,
 * push_back() combine with the pop_front().
 * The overhead is a tail pointer in the header sentinel that is very cheap for some scenarios.
 * e.g. Output buffer for network library: consume them used buffer in the front and push
 * new contents to the back of the list.
 */
typedef struct zda_delist {
    zda_slist_t        list;
    zda_delist_node_t *tail_before;
} zda_delist_t;

static zda_inline void zda_delist_init(zda_delist_t *dl) zda_noexcept
{
    zda_slist_header_init(&dl->list);
    dl->tail_before = &dl->list.node;
}

static zda_inline void zda_delist_move(zda_delist_t *dl, zda_delist_t *rhs) zda_noexcept
{
    zda_slist_move(&dl->list, &rhs->list);
    dl->tail_before = rhs->tail_before;

    rhs->tail_before = &dl->list.node;
}

static zda_inline void zda_delist_swap(zda_delist_t *dl, zda_delist_t *rhs) zda_noexcept
{
    zda_slist_swap(&dl->list, &rhs->list);
    zda_swap(&dl->tail_before, &rhs->tail_before, zda_delist_node_t *);
}

static zda_inline zda_bool zda_delist_is_empty(zda_delist_t const *dl) zda_noexcept
{
    return (zda_slist_is_empty(&dl->list));
}

static zda_inline zda_bool zda_delist_is_single(zda_delist_t const *dl) zda_noexcept
{
    return zda_slist_is_single(&dl->list);
}

static zda_inline zda_delist_node_t const *zda_delist_front_const(zda_delist_t const *dl
) zda_noexcept
{
    return zda_slist_front((zda_slist_t *)(&dl->list));
}

static zda_inline zda_delist_node_t *zda_delist_front(zda_delist_t *dl) zda_noexcept
{
    return zda_slist_front(&dl->list);
}

static zda_inline zda_delist_node_t *zda_delist_back(zda_delist_t *dl) zda_noexcept
{
    return dl->tail_before->next;
}

static zda_inline zda_delist_node_t const *zda_delist_back_const(zda_delist_t const *dl
) zda_noexcept
{
    return dl->tail_before->next;
}
/****************************/
/* Insert APIs */
/****************************/
static zda_inline void zda_delist_insert_after(
    zda_delist_t      *dl,
    zda_delist_node_t *pos,
    zda_delist_node_t *node
) zda_noexcept
{
    if (pos == dl->tail_before) {
        dl->tail_before = node;
    }
    zda_slist_insert_after(pos, node);
}

static zda_inline void zda_delist_push_front(zda_delist_t *dl, zda_delist_node_t *node) zda_noexcept
{
    zda_delist_insert_after(dl, zda_slist_get_header_node(&dl->list), node);
}

static zda_inline void zda_delist_push_back(zda_delist_t *dl, zda_delist_node_t *node) zda_noexcept
{
    zda_delist_insert_after(dl, dl->tail_before, node);
}

/*******************************/
/* Remove APIs */
/*******************************/

static zda_inline zda_delist_node_t *zda_delist_remove_after(
    zda_delist_t      *dl,
    zda_delist_node_t *pos
) zda_noexcept
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
        dl->tail_before = &dl->list.node;
    }
    zda_delist_node_t *ret = zda_slist_pop_front(&dl->list);
    return ret;
}

/* The single tail node is not enough to implement the pop_back.
 * Even though record the tail_before_before, the before_before_before is also required in the next
 * pop_back(). The only solution is use the list instead delist. */
/* static zda_inline zda_delist_node_t *zda_delist_pop_back(zda_delist_t *dl) zda_noexcept
{
} */

/*****************************/
/* Iterator APIs */
/*****************************/

zda_inline zda_delist_node_t *zda_delist_get_first(zda_delist_t *dl) zda_noexcept
{
    return dl->list.node.next;
}

zda_inline zda_delist_node_t const *zda_delist_get_first_const(zda_delist_t const *dl) zda_noexcept
{
    return dl->list.node.next;
}

zda_inline zda_delist_node_t *zda_delist_get_terminator(zda_delist_t *dl) zda_noexcept
{
    return NULL;
}

zda_inline zda_delist_node_t const *zda_delist_get_terminator_const(zda_delist_t const *dl
) zda_noexcept
{
    return NULL;
}

zda_inline zda_delist_node_t *zda_delist_get_next(zda_delist_node_t *node) { return node->next; }

zda_inline zda_bool zda_delist_is_terminator(zda_delist_node_t const *node) zda_noexcept
{
    return node == NULL;
}

#define zda_delist_iterate(dl)                                                                     \
    for (zda_delist_node_t *pos = (dl)->list.node.next; pos != NULL; pos = pos->next)

/*****************************/
/* Search APIs */
/*****************************/

#define zda_delist_search_inplace(header, val, type, member, cmp, p_entry)                         \
    do {                                                                                           \
        zda_delist_iterate(header)                                                                 \
        {                                                                                          \
            type *entry = zda_delist_entry(pos, type);                                             \
            if (cmp(entry->member, val)) {                                                         \
                p_entry = entry;                                                                   \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
    } while (0)

/***************************/
/* Destroy APIs            */
/***************************/
#define zda_delist_destroy_inplace(delist, type, free_cb)                                          \
    do {                                                                                           \
        zda_delist_node_t *next;                                                                   \
        for (zda_delist_node_t *pos = zda_slist_front(&((delist)->list)); pos != NULL;) {          \
            next = pos->next;                                                                      \
            free_cb(zda_delist_entry(pos, type));                                                  \
            pos = next;                                                                            \
        }                                                                                          \
    } while (0)

#endif /* guard */
