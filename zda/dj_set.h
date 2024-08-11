// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_DJ_SET_H__
#define _ZDA_DJ_SET_H__

#include "util/macro.h"

// @FYI [CRLS 4th] ch19 Data Structures for Disjoint Sets

/**
 * @brief The disjoint set representive
 * The node don't hold the children pointers.
 * The FIND_SET procedure only interest in the path of parent to root.
 */
typedef struct zda_dj_set_node {
    struct zda_dj_set_node *parent;
    size_t rank;
} zda_dj_set_node_t;

#define ZDA_DJ_SET_HOOK zda_dj_set_node_t node;

EXTERN_C_BEGIN

static zda_inline void zda_dj_set_init(zda_dj_set_node_t *node) zda_noexcept
{
    node->parent = node;
    node->rank = 0;
}

/**
 * @brief Find the root node of the \p node belonging set
 */
static zda_inline zda_dj_set_node_t *
zda_dj_set_find(zda_dj_set_node_t *node) zda_noexcept
{
    while (node->parent != node) {
        node = node->parent;
    }
    return node;
}

/**
 * @brief Union the two set where the \p lnode and \p rnode belonging
 * @return 
 * NULL - the two node are belonging same set
 * otherwise, the root of new set
 */
zda_dj_set_node_t *
zda_dj_set_union(zda_dj_set_node_t *lnode,
                 zda_dj_set_node_t *rnode) zda_noexcept;


EXTERN_C_END

#endif // Header Guard