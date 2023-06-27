// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _ZDA_ASSERT_H_
#define _ZDA_ASSERT_H_

#include <assert.h>

#define zda_assert0(cond)     assert((cond))
#define zda_assert(cond, msg) assert((cond) && (msg));

#endif /* Header guard */