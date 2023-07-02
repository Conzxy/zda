#include "zda/avl_ht.h"
#include "zda/avl_tree.h"
#include "zda/util/macro.h"
#include <stdio.h>

zda_avl_ht_iter_t zda_avl_ht_get_first(zda_avl_ht_t *ht) zda_noexcept
{
  for (size_t i = 0; i < ht->bkt_capa; ++i) {
    zda_avl_ht_list_t *hlist = &ht->tb[i];
    zda_avl_ht_node_t *first = zda_avl_tree_get_first(hlist);
    if (first) {
      return _zda_avl_ht_mk_iter(ht, first, i);
    }
  }
  return zda_avl_ht_get_terminator(ht);
}

void zda_avl_ht_iter_inc(zda_avl_ht_iter_t *iter) zda_noexcept
{
  assert(!zda_avl_ht_iter_is_terminator(iter));
  zda_avl_ht_node_t *next = zda_avl_node_get_next(iter->node);
  if (next) {
    iter->node = next;
    return;
  }
  zda_avl_ht_t *ht = iter->ht;
  for (size_t i = iter->idx + 1; i < ht->bkt_capa; ++i) {
    zda_avl_ht_list_t *hlist = &ht->tb[i];
    zda_avl_ht_node_t *next  = hlist->node;
    if (next) {
      iter->node = next;
      iter->idx  = i;
      return;
    }
  }
  iter->node = NULL;
}

void zda_avl_ht_print_layout(zda_avl_ht_t *ht, void (*print_cb)(zda_avl_ht_node_t *node))
    zda_noexcept
{
  printf("Bucket count = %zu\n", ht->bkt_capa);
  printf("Entry count = %zu\n", ht->cnt);
  printf("Mask = %zu\n", ht->mask);

  for (size_t i = 0; i < ht->bkt_capa; ++i) {
    printf("[%zu]: ", i);
    zda_avl_ht_list_t *hlist = &ht->tb[i];
    for (zda_avl_ht_node_t *pos = zda_avl_tree_get_first(hlist); pos != NULL;
         pos                    = zda_avl_node_get_next(pos))
    {
      print_cb(pos);
      printf("->");
    }
    printf("NULL\n");
  }
}