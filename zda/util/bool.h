#ifndef _ZDA_UTIL_BOOL_H__
#define _ZDA_UTIL_BOOL_H__

/* The <stdbool.h> is introduced in IOS C99.
 * I can't use it in IOS C89. */
typedef unsigned char zda_bool;

#define zda_true  (zda_bool)1
#define zda_false (zda_bool)0

#endif /* Header guard */