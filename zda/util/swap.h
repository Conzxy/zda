#ifndef _ZDA_UTIL_SWAP_H__
#define _ZDA_UTIL_SWAP_H__

#define zda_swap(lhs, rhs, type)                                                                   \
    do {                                                                                           \
        type *__lhs = lhs;                                                                         \
        type *__rhs = rhs;                                                                         \
        type *tmp   = __rhs;                                                                       \
        *__rhs      = *__lhs;                                                                      \
        *__lhs      = *tmp;                                                                        \
    } while (0)

#endif
