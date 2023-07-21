#ifndef _ZDA_STRING_VIEW_UTIL_HPP__
#define _ZDA_STRING_VIEW_UTIL_HPP__

#include "zda/result.hpp"
#include "zda/string_view.hpp"
#include "zda/string_view_util.h"

namespace zda {

#define __DEF_STRING_VIEW_TO_INT__(type, suffix) \
static zda_inline Result<type> string_view_to_##suffix(StringView sv, int base = 10) zda_noexcept \
{ \
    type tmp; \
    auto res = zda_string_view_to_##suffix(*sv.rep(), base, &tmp); \
    if (res) { \
        return tmp; \
    } \
    return RES_NULL; \
}

__DEF_STRING_VIEW_TO_INT__(uint64_t, u64)
__DEF_STRING_VIEW_TO_INT__(uint32_t, u32)
__DEF_STRING_VIEW_TO_INT__(uint16_t, u16)
__DEF_STRING_VIEW_TO_INT__(uint8_t, u8)

__DEF_STRING_VIEW_TO_INT__(int64_t, s64)
__DEF_STRING_VIEW_TO_INT__(int32_t, s32)
__DEF_STRING_VIEW_TO_INT__(int16_t, s16)
__DEF_STRING_VIEW_TO_INT__(int8_t, s8)

}

#endif