// SPDX-LICENSE-IDENTIFIER: MIT
#include "zda/ht.h"
#include <stdio.h>

#define ZDA_HT_BKT_CNT_TB_SIZE (sizeof(size_t) << 3)

static size_t _zda_ht_bkt_cnts[ZDA_HT_BKT_CNT_TB_SIZE + 1] = {
    0,
    (((size_t)1) << 0),
    (((size_t)1) << 1),
    (((size_t)1) << 2),
    (((size_t)1) << 3),
    (((size_t)1) << 4),
    (((size_t)1) << 5),
    (((size_t)1) << 6),
    (((size_t)1) << 7),
    (((size_t)1) << 8),
    (((size_t)1) << 9),
    (((size_t)1) << 10),
    (((size_t)1) << 11),
    (((size_t)1) << 12),
    (((size_t)1) << 13),
    (((size_t)1) << 14),
    (((size_t)1) << 15),
    (((size_t)1) << 16),
    (((size_t)1) << 17),
    (((size_t)1) << 18),
    (((size_t)1) << 19),
    (((size_t)1) << 20),
    (((size_t)1) << 21),
    (((size_t)1) << 22),
    (((size_t)1) << 23),
    (((size_t)1) << 24),
    (((size_t)1) << 25),
    (((size_t)1) << 26),
    (((size_t)1) << 27),
    (((size_t)1) << 28),
    (((size_t)1) << 29),
    (((size_t)1) << 30),
    (((size_t)1) << 31),
    (((size_t)1) << 32),
    (((size_t)1) << 33),
    (((size_t)1) << 34),
    (((size_t)1) << 35),
    (((size_t)1) << 36),
    (((size_t)1) << 37),
    (((size_t)1) << 38),
    (((size_t)1) << 39),
    (((size_t)1) << 40),
    (((size_t)1) << 41),
    (((size_t)1) << 42),
    (((size_t)1) << 43),
    (((size_t)1) << 44),
    (((size_t)1) << 45),
    (((size_t)1) << 46),
    (((size_t)1) << 47),
    (((size_t)1) << 48),
    (((size_t)1) << 49),
    (((size_t)1) << 50),
    (((size_t)1) << 51),
    (((size_t)1) << 52),
    (((size_t)1) << 53),
    (((size_t)1) << 54),
    (((size_t)1) << 55),
    (((size_t)1) << 56),
    (((size_t)1) << 57),
    (((size_t)1) << 58),
    (((size_t)1) << 59),
    (((size_t)1) << 60),
    (((size_t)1) << 61),
    (((size_t)1) << 62),
    (((size_t)1) << 63),
};

static size_t _zda_ht_get_nearest_cnt(size_t n)
{
  // Just >= n
  size_t l = 0;
  size_t r = ZDA_HT_BKT_CNT_TB_SIZE - 1;
  size_t m;

  while (l < r) {
    m = (r - l) / 2 + l;
    if (n <= _zda_ht_bkt_cnts[m]) {
      r = m;
    } else {
      l = m + 1;
    }
  }

  return _zda_ht_bkt_cnts[l];
}

int zda_ht_reserve_init(zda_ht_t *ht, size_t n)
{
  n                       = _zda_ht_get_nearest_cnt(n);
  zda_ht_list_t *new_list = (zda_ht_list_t *)realloc(ht->tb, sizeof(zda_ht_list_t) * n);
  if (!new_list) return 0;
  for (size_t i = 0; i < n; ++i) {
    new_list[i].node.next = NULL;
  }
  ht->tb       = new_list;
  ht->bkt_capa = n;
  ht->mask     = ht->bkt_capa - 1;
  return 1;
}

void zda_ht_print_layout(zda_ht_t *ht, void (*print_cb)(zda_ht_node_t *node))
{
  printf("Bucket count = %zu\n", ht->bkt_capa);
  printf("Entry count = %zu\n", ht->cnt);
  printf("Mask = %zu\n", ht->mask);

  for (size_t i = 0; i < ht->bkt_capa; ++i) {
    printf("[%zu]: ", i);
    zda_ht_list_t *hlist = &ht->tb[i];
    for (zda_ht_node_t *pos = hlist->node.next; pos != NULL; pos = pos->next) {
      print_cb(pos);
      printf("->");
    }
    printf("NULL\n");
  }
}

void zda_ht_insert_commit(zda_ht_t *ht, zda_ht_commit_ctx_t *p_ctx, zda_ht_node_t *node)
{
  zda_ht_insert_commit_inplace(ht, *p_ctx, node);
}

void zda_ht_iter_inc(zda_ht_iter_t *iter)
{
  assert(!zda_ht_iter_is_terminator(iter));
  zda_ht_node_t *next = iter->node->next;
  if (next) {
    iter->node = next;
    return;
  }
  zda_ht_t *ht = iter->ht;
  for (size_t i = iter->idx + 1; i < ht->bkt_capa; ++i) {
    zda_ht_list_t *hlist = &ht->tb[i];
    zda_ht_node_t *next  = hlist->node.next;
    if (next) {
      iter->node = next;
      iter->idx  = i;
      return;
    }
  }
  iter->node = NULL;
}

zda_ht_iter_t zda_ht_get_first(zda_ht_t *ht)
{
  for (size_t i = 0; i < ht->bkt_capa; ++i) {
    zda_ht_list_t  hlist = ht->tb[i];
    zda_ht_node_t *first = hlist.node.next;
    if (first) {
      return _zda_ht_mk_iter(ht, first, i);
    }
  }
  return zda_ht_get_terminator(ht);
}