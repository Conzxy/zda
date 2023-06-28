#ifndef _ZDA_UTIL_FUNCTOR_HPP__
#define _ZDA_UTIL_FUNCTOR_HPP__

#include <cstdlib>
#include "zda/util/macro.h"
#include "zda/zstl/destroy.hpp"

namespace zda {

template <typename Entry>
struct LibcFree {
  zda_inline void operator()(Entry *e) noexcept
  {
    zstl::Destroy(e);
    free(e);
  }
};

struct EmptyFree {
  zda_inline void operator()(void *) noexcept {}
};

template <typename V, typename NodeType>
struct VEntry {
  V        value;
  NodeType node;
};

} // namespace zda

#endif
