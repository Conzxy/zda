// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_MACRO_H__
#define _ZDA_MACRO_H__

#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END   }

#ifndef INLINE
#  if defined(__GNUC__)
#    define INLINE __inline__

#    if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#      define ENABLE_ALWAYS_INLINE
#    endif
#  elif (defined(_MSC_VER) || defined(__WATCOMC__))
#    define INLINE __inline
#  else
#    define INLINE
#  endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#  define inline INLINE
#endif /* ! INLINE */

#ifdef ENABLE_ALWAYS_INLINE
#  define zda_inline inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#  define zda_inline inline __forceinline
#endif

#include <stddef.h>

#define ZDA_NULL NULL

/* The `typeof()` is a extension, I don't use it here */
#define zda_max(x, y) (x > y ? x : y)
#define zda_min(x, y) (x < y ? x : y)

#endif /* Header Guard */