#include "zda/dj_set.h"

zda_dj_set_node_t *zda_dj_set_union(zda_dj_set_node_t *lnode,
                                    zda_dj_set_node_t *rnode) zda_noexcept
{
    zda_dj_set_node_t *left = zda_dj_set_find(lnode);
    zda_dj_set_node_t *right = zda_dj_set_find(rnode);
    
    if (left == right) return NULL;

    zda_dj_set_node_t *new_root = NULL;
    if (left->rank < right->rank) {
        left->parent = right;
        new_root = right;
    } else {
        right->parent = left;
        if (left->rank == right->rank) {
            left->rank++;
        }
        new_root = left;
    }
    return new_root;
}