#ifndef _ZDA_UTIL_COMPARATOR_HPP__
#define _ZDA_UTIL_COMPARATOR_HPP__

#include <string>
#include <cstring>
#include <cstdint>

#include "zda/util/macro.h"

namespace zda {

template <typename T>
struct Comparator {
  static_assert(sizeof(T) < 0, "The specialization of Comparator<T> isn't defined");
};

#define ZDA_DEF_COMPARATOR_SPEC_SIGNED_(t_)                                                        \
  template <>                                                                                      \
  struct Comparator<t_> {                                                                          \
    zda_inline int operator()(t_ x, t_ y) const noexcept                                           \
    {                                                                                              \
      return x - y;                                                                                \
    }                                                                                              \
  }

ZDA_DEF_COMPARATOR_SPEC_SIGNED_(int8_t);
ZDA_DEF_COMPARATOR_SPEC_SIGNED_(int16_t);
ZDA_DEF_COMPARATOR_SPEC_SIGNED_(int32_t);
ZDA_DEF_COMPARATOR_SPEC_SIGNED_(int64_t);
// ZDA_DEF_COMPARATOR_SPEC_SIGNED_(int);

/* To unsigned integer,
 * can't return x - y since
 * (signed)(x - y(y > 2^31-1)) > 0
 */
#define ZDA_DEF_COMPARATOR_SPEC_UNSIGNED_(t_)                                                      \
  template <>                                                                                      \
  struct Comparator<t_> {                                                                          \
    zda_inline int32_t operator()(t_ x, t_ y) const noexcept                                       \
    {                                                                                              \
      return (x < y) ? -1 : ((x == y) ? 0 : 1);                                                    \
    }                                                                                              \
  }

ZDA_DEF_COMPARATOR_SPEC_UNSIGNED_(uint8_t);
ZDA_DEF_COMPARATOR_SPEC_UNSIGNED_(uint16_t);
ZDA_DEF_COMPARATOR_SPEC_UNSIGNED_(uint32_t);
ZDA_DEF_COMPARATOR_SPEC_UNSIGNED_(uint64_t);

template <typename Alloc>
struct Comparator<std::basic_string<char, std::char_traits<char>, Alloc>> {
  using StrType = std::basic_string<char, std::char_traits<char>, Alloc>;
  zda_inline int operator()(StrType const &x, StrType const &y) const noexcept
  {
    return ::strcmp(x.c_str(), y.c_str());
  }
};

template <>
struct Comparator<double> {
  zda_inline int operator()(double x, double y) const noexcept
  {
    return (x < y) ? -1 : ((x == y) ? 0 : 1);
  }
};

template <>
struct Comparator<float> {
  zda_inline int operator()(float x, float y) const noexcept
  {
    return (x < y) ? -1 : ((x == y) ? 0 : 1);
  }
};

template <typename T>
struct Comparator<T *> {
  zda_inline int operator()(T *const x, T *const y) const noexcept { return x - y; }
};

} // namespace zda

#endif
