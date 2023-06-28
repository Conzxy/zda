#ifndef _ZSTL_DESTROY_HPP__
#define _ZSTL_DESTROY_HPP__

#include "zda/zstl/type_traits.h"

namespace zstl {

template <typename T, zstl::enable_if_t<std::is_trivial<T>::value, int> = 0>
inline void Destroy(T *obj) noexcept
{
}

template <typename T, zstl::enable_if_t<!std::is_trivial<T>::value, char> = 0>
inline void Destroy(T *obj)
{
  obj->~T();
}

} // namespace zstl

#endif
