// SPDX-LICENSE-IDENTIFIER: MIT
#include "zda/slist.h"

zda_slist_node_t *zda_slist_search(zda_slist_header_t *header, void const *key, zda_slist_cmp_t cmp)
{
  zda_slist_iterate(header)
  {
    if (cmp(pos, key)) {
      return pos;
    }
  }
  return NULL;
}

zda_slist_node_t *zda_slist_search2(
    zda_slist_header2_t *header,
    void const          *key,
    zda_slist_cmp_t      cmp
)
{
  zda_slist_iterate2(header)
  {
    if (cmp(pos, key)) {
      return pos;
    }
  }
  return NULL;
}
