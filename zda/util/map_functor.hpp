#ifndef _ZDA_MAP_FUNCTOR_HPP__
#define _ZDA_MAP_FUNCTOR_HPP__

#include <zda/util/macro.h>

namespace zda {

template <typename K, typename V, typename NodeType>
struct KvEntry {
  K        key;
  V        value;
  NodeType node;
};

template <typename K, typename NodeType>
struct KEntry {
  K        key;
  NodeType node;
};

template <typename Entry, typename K>
struct GetKey {
  zda_inline K operator()(Entry const *entry) const noexcept { return entry->key; }
};

} // namespace zda

#endif