// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_CONTAINER_OF_H__
#define _ZDA_CONTAINER_OF_H__

#include <stdlib.h>

#ifndef offsetof
#  define offsetof(type, member) (size_t)(&((type *)0)->member)
#endif

#ifndef container_of
#  if defined(__GNUC__) || defined(__clang__)
/* The implementation ensures the type safety:
 * If the type of ptr is not same with the member pointer,
 * the compiler will refuse it and trigger error
 *
 * The `typeof()` is a extension operator of `GCC`,
 * to other compilers, don't check.
 *
 * Specially, `typeof()` can be replaced by `decltype()` of C++.
 * This is a standard operator so can be used in any good C++ compiler. */
#    define container_of(ptr, type, member)                                                        \
      ({                                                                                           \
        typeof(((type *)0)->member) *__p_member = ptr;                                             \
        (type *)((unsigned char *)__p_member - offsetof(type, member));                            \
      })

/* The `typeof()` cannot be checked by `#if defined(typeof)`,
 * so define a macro to replace `typeof()` is difficult. */
#  elif defined(__cplusplus)
#    define container_of(ptr, type, member)                                                        \
      ({                                                                                           \
        decltype(((type *)0)->member) *__p_member = ptr;                                           \
        (type *)((unsigned char *)__p_member - offsetof(type, member));                            \
      })
#  else
#    define container_of(ptr, type, member) (type *)((unsigned char *)addr - offset(type, member))
#  endif /* if defined(__GUNC__) */

#endif /* #ifndef container_of */

#endif /* Header guard */