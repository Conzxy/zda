#include "zda/string_view_util.h"
#include "zda/util/assert.h"

#include <stdlib.h>

zda_bool zda_string_view_to_u64(zda_string_view_t sv, int base, uint64_t *o_u64) zda_noexcept
{
    zda_assert0(sv.len < 64);
    static char buf[64];
    memcpy(buf, sv.data, sv.len);
    buf[sv.len] = 0;
    char *end = NULL;
    uint64_t ret = (uint64_t)strtoull(buf, &end, base);
    if (ret == 0 && end == buf) {
        return zda_false;
    }
    *o_u64 = ret;
    return zda_true;
}

zda_bool zda_string_view_to_u32(zda_string_view_t sv, int base, uint32_t *o_u32) zda_noexcept
{
    zda_assert0(sv.len < 32);
    static char buf[32];
    memcpy(buf, sv.data, sv.len);
    buf[sv.len] = 0;
    char *end = NULL;
    uint32_t ret = (uint32_t)strtoul(buf, &end, base);
    if (ret == 0 && end == buf) {
        return zda_false;
    }
    *o_u32 = ret;
    return zda_true;
}

zda_bool zda_string_view_to_s64(zda_string_view_t sv, int base, int64_t *o_64) zda_noexcept
{
    zda_assert0(sv.len < 64);
    static char buf[64];
    memcpy(buf, sv.data, sv.len);
    buf[sv.len] = 0;
    char *end = NULL;
    int64_t ret = (int64_t)strtoll(buf, &end, base);
    if (ret == 0 && end == buf) {
        return zda_false;
    }
    *o_64 = ret;
    return zda_true;
}

zda_bool zda_string_view_to_s32(zda_string_view_t sv, int base, int32_t *o_32) zda_noexcept
{
    zda_assert0(sv.len < 32);
    static char buf[32];
    memcpy(buf, sv.data, sv.len);
    buf[sv.len] = 0;
    char *end = NULL;
    int32_t ret = (int32_t)strtol(buf, &end, base);
    if (ret == 0 && end == buf) {
        return zda_false;
    }
    *o_32 = ret;
    return zda_true;
}