#ifndef _ZDA_UTIL_FUNCTOR_HPP__
#define _ZDA_UTIL_FUNCTOR_HPP__

#include <cstdlib>
#include "zda/util/macro.h"

namespace zda {

struct LibcFree {
  zda_inline void operator()(void *e) zda_noexcept { free(e); }
};

struct EmptyFree {
  zda_inline void operator()(void *) zda_noexcept {}
};

} // namespace zda

#endif
