#ifndef _ZDA_STRING_VIEW_UTIL_H__
#define _ZDA_STRING_VIEW_UTIL_H__

#include "zda/util/export.h"
#include "zda/string_view.h"
#include "zda/util/macro.h"
#include <stdint.h>

#ifdef __cplusplus
EXTERN_C_BEGIN
#endif

ZDA_API zda_bool zda_string_view_to_u64(zda_string_view_t sv, int base, uint64_t *o_u64) zda_noexcept;
ZDA_API zda_bool zda_string_view_to_u32(zda_string_view_t sv, int base, uint32_t *o_u32) zda_noexcept;
ZDA_API zda_bool zda_string_view_to_s64(zda_string_view_t sv, int base, int64_t *o_64) zda_noexcept;
ZDA_API zda_bool zda_string_view_to_s32(zda_string_view_t sv, int base, int32_t *o_32) zda_noexcept;

static zda_inline zda_bool zda_string_view_to_u16(zda_string_view_t sv, int base, uint16_t *o_) zda_noexcept
{
    uint32_t tmp;
    zda_bool res =  zda_string_view_to_u32(sv, base, &tmp);
    if (res && tmp <= 0xffffffff) {
        *o_ = tmp;
        return zda_true;
    }
    return zda_false;
}

static zda_inline zda_bool zda_string_view_to_s16(zda_string_view_t sv, int base, int16_t *o_) zda_noexcept
{
    int32_t tmp;
    zda_bool res =  zda_string_view_to_s32(sv, base, &tmp);
    if (res && tmp <= 0x7fffffff && tmp >= 0x80000000) {
        *o_ = tmp;
        return zda_true;
    }
    return zda_false;
}

static zda_inline zda_bool zda_string_view_to_u8(zda_string_view_t sv, int base, uint8_t *o_) zda_noexcept
{ 
    uint32_t tmp;
    zda_bool res =  zda_string_view_to_u32(sv, base, &tmp);
    if (res && tmp <= 0xffff) {
        *o_ = tmp;
        return zda_true;
    }
    return zda_false;
}

static zda_inline zda_bool zda_string_view_to_s8(zda_string_view_t sv, int base, int8_t *o_) zda_noexcept
{
    int32_t tmp;
    zda_bool res =  zda_string_view_to_s32(sv, base, &tmp);
    if (res && tmp <= 0x7fff && tmp >= 0x8000) {
        *o_ = tmp;
        return zda_true;
    }
    return zda_false;
}

#ifdef __cplusplus
EXTERN_C_END
#endif

#endif