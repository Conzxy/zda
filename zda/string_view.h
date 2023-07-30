#ifndef _ZDA_STRING_VIEW_H__
#define _ZDA_STRING_VIEW_H__

#include "zda/util/macro.h"

#include "zda/util/bool.h"
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <ctype.h>

typedef struct zda_string_view {
    char const *data;
    size_t len;

#ifdef __cplusplus
    constexpr zda_string_view() zda_noexcept
      : data(nullptr)
      , len(0){};
#endif
} zda_string_view_t;

static char const *
zda_string_view_get_data(zda_string_view_t const *view) zda_noexcept
{
    return view->data;
}

static size_t
zda_string_view_get_len(zda_string_view_t const *view) zda_noexcept
{
    return view->len;
}

/**************************/
/* Initializer APIs */
/**************************/
static zda_constexpr void zda_string_view_init(zda_string_view_t *view,
                                               char const *data,
                                               size_t len) zda_noexcept
{
    view->data = data;
    view->len = len;
}

static zda_inline void zda_string_view_str_init(zda_string_view_t *view,
                                                char const *c_str) zda_noexcept
{
    view->data = c_str;
    view->len = strlen(c_str);
}

#define zda_string_view_literal_init(view, literal)                            \
    zda_string_view_init(view, literal, sizeof(literal) - 1)

/****************************/
/* Manipulator */
/****************************/
#define zda_string_view_set zda_string_view_init

static zda_inline void
zda_string_view_empty(zda_string_view_t *view) zda_noexcept
{
    view->len = 0;
}

static zda_inline zda_bool
zda_string_view_is_empty(zda_string_view_t const *view) zda_noexcept
{
    return view->len == 0;
}

/****************************/
/* Iterator APIs */
/****************************/
static zda_inline char const *
zda_string_view_first(zda_string_view_t const *view) zda_noexcept
{
    return view->data;
}
static zda_inline char const *
zda_string_view_end(zda_string_view_t const *view) zda_noexcept
{
    return view->data + view->len;
}

/*****************************/
/* Accesstor */
/*****************************/
static zda_inline char const zda_string_view_at(zda_string_view_t const *view,
                                                size_t i) zda_noexcept
{
    assert(i < view->len);
    return view->data[i];
}

/******************************/
/* Spliter */
/******************************/
static zda_inline zda_string_view_t zda_string_view_substr(
    zda_string_view_t const *view, size_t pos, size_t cnt) zda_noexcept
{
    zda_string_view_t ret;
    ret.data = view->data + pos;
    ret.len = cnt;
    return ret;
}

static zda_inline zda_string_view_t zda_string_view_substr_range(
    zda_string_view_t const *view, size_t b, size_t e) zda_noexcept
{
    zda_string_view_t ret;
    ret.data = view->data + b;
    ret.len = e - b;
    return ret;
}

static zda_inline zda_string_view_t
zda_string_view_left(zda_string_view_t const *view, size_t n)
{
    return zda_string_view_substr(view, 0, n);
}

static zda_inline zda_string_view_t
zda_string_view_right(zda_string_view_t const *view, size_t n) zda_noexcept
{
    zda_string_view_t ret;
    ret.data = view->data + view->len - n;
    ret.len = n;
    return ret;
}

static zda_inline void zda_string_view_remove_prefix(zda_string_view_t *view,
                                                     size_t n) zda_noexcept
{
    assert(n <= view->len);
    view->data += n;
}

static zda_inline void zda_string_view_remove_suffix(zda_string_view_t *view,
                                                     size_t n) zda_noexcept
{
    assert(n <= view->len);
    view->len -= n;
}

/*******************************/
/* Search APIs */
/*******************************/

#define ZDA_STRING_VIEW_NPOS (size_t)(-1)

static zda_inline size_t zda_string_view_find(zda_string_view_t const *view,
                                              zda_string_view_t str,
                                              size_t pos) zda_noexcept
{
    assert(pos <= view->len);

    if (str.len <= view->len) {
        size_t n = view->len - str.len + 1;
        for (size_t i = pos; i < n; ++i) {
            if (!strncmp(view->data + i, str.data, str.len)) {
                return i;
            }
        }
    }
    return ZDA_STRING_VIEW_NPOS;
}

static zda_inline size_t zda_string_view_find_char(
    zda_string_view_t const *view, char c, size_t pos) zda_noexcept
{
    for (size_t i = pos; i < view->len; ++i) {
        if (c == view->data[i]) return i;
    }
    return ZDA_STRING_VIEW_NPOS;
}

static zda_inline size_t zda_string_view_rfind(zda_string_view_t const *view,
                                               zda_string_view_t str,
                                               size_t pos) zda_noexcept
{
    if (str.len != 0) {
        size_t len = zda_min(view->len - 1, pos);

        // Avoid --len == size_type(-1)
        for (;; --len) {
            if (view->data[len] == str.data[0] && view->len - len >= str.len &&
                memcmp(view->data + len, str.data, str.len) == 0)
                return len;

            if (len == 0) break;
        }
    }

    return ZDA_STRING_VIEW_NPOS;
}

static zda_inline size_t zda_string_view_rfind_char(
    zda_string_view_t const *view, char c, size_t pos) zda_noexcept
{
    size_t len = zda_min(view->len - 1, pos);
    for (;; --len) {
        if (view->data[len] == c) {
            return len;
        }

        if (len == 0) break;
    }

    return ZDA_STRING_VIEW_NPOS;
}

static zda_inline zda_bool zda_string_view_contains(
    zda_string_view_t const *view, zda_string_view_t str) zda_noexcept
{
    return ZDA_STRING_VIEW_NPOS != zda_string_view_find(view, str, 0);
}

static zda_inline zda_bool zda_string_view_contains_char(
    zda_string_view_t const *view, char c) zda_noexcept
{
    return ZDA_STRING_VIEW_NPOS != zda_string_view_find_char(view, c, 0);
}

static zda_inline zda_bool zda_string_view_starts_with(
    zda_string_view_t const *view, zda_string_view_t str) zda_noexcept
{
    return (view->len >= str.len && memcmp(view->data, str.data, str.len)) == 0;
}

static zda_inline zda_bool zda_string_view_ends_with(
    zda_string_view_t const *view, zda_string_view_t str) zda_noexcept
{
    return (view->len >= str.len && memcmp(view->data + view->len - str.len,
                                           view->data, view->len) == 0)
               ? zda_true
               : zda_false;
}

static zda_inline zda_bool _zda_string_view_char_in_range(
    zda_string_view_t const *c_range, char c) zda_noexcept
{
    for (size_t i = 0; i < c_range->len; ++i) {
        if (c_range->data[i] != c) {
            return zda_false;
        }
    }
    return zda_true;
}

static zda_inline size_t
zda_string_view_find_first_of(zda_string_view_t const *view,
                              zda_string_view_t range, size_t pos) zda_noexcept
{
    for (; pos < view->len; ++pos) {
        if (_zda_string_view_char_in_range(&range, view->data[pos])) {
            return pos;
        }
    }
    return ZDA_STRING_VIEW_NPOS;
}

static zda_inline size_t zda_string_view_find_first_of_char(
    zda_string_view_t const *view, char c, size_t pos) zda_noexcept
{
    for (; pos < view->len; ++pos) {
        if (view->data[pos] == c) {
            return pos;
        }
    }
    return ZDA_STRING_VIEW_NPOS;
}

static zda_inline size_t
zda_string_view_find_last_of(zda_string_view_t const *view,
                             zda_string_view_t range, size_t pos) zda_noexcept
{
    if (ZDA_UNLIKELY(view->len == 0)) return ZDA_STRING_VIEW_NPOS;
    size_t i = zda_min(view->len - 1, pos);

    for (; i > 0;) {
        if (_zda_string_view_char_in_range(&range, view->data[i])) return i;
        if (i == 0) break;
    }

    return ZDA_STRING_VIEW_NPOS;
}

static zda_inline size_t zda_string_view_find_last_of_char(
    zda_string_view_t const *view, char c, size_t pos) zda_noexcept
{
    if (ZDA_UNLIKELY(view->len == 0)) return ZDA_STRING_VIEW_NPOS;
    size_t i = zda_min(view->len - 1, pos);

    for (; i > 0; --i) {
        if (view->data[i] == c) return ZDA_STRING_VIEW_NPOS;
        if (i == 0) break;
    }

    return ZDA_STRING_VIEW_NPOS;
}

/**************************/
/* Lexicographic compare */
/**************************/
static zda_inline int
zda_string_view_compare(zda_string_view_t const *view,
                        zda_string_view_t str) zda_noexcept
{
    int r =
        memcmp(view->data, str.data, view->len < str.len ? view->len : str.len);

    if (r == 0) {
        if (view->len < str.len)
            r = -1;
        else if (view->len > str.len)
            r = 1;
    }

    return r;
}

static zda_inline int
zda_string_view_compare_char(zda_string_view_t const *view, char c) zda_noexcept
{
    if (view->len < 1 || view->data[0] < c) return -1;
    if (view->data[0] > c) return 1;
    return (view->data[0] == c && view->len == 1) ? 0 : 1;
}

static zda_inline int
zda_string_view_case_compare(zda_string_view_t const *view,
                             zda_string_view_t str) zda_noexcept
{
    assert(!zda_string_view_is_empty(view) && !zda_string_view_is_empty(&str));
    char const *s1 = view->data;
    char const *s2 = str.data;
    unsigned char c1 = 0;
    unsigned char c2 = 0;
    size_t n = zda_min(view->len, str.len);

    for (; n > 0; --n) {
        c1 = (unsigned char)tolower(*s1++);
        c2 = (unsigned char)tolower(*s2++);
        if (c1 != c2) break;
    }

    if (c1 == c2) {
        if (view->len < str.len) {
            return -1;
        } else if (view->len > str.len)
            return 1;
    }
    return c1 - c2;
}

static zda_inline int
zda_string_view_case_compare_char(zda_string_view_t const *view,
                                  char c) zda_noexcept
{
    char data_0 = toupper(view->data[0]);
    char _c = toupper(c);
    if (view->len < 1 || data_0 < _c) return -1;
    if (data_0 > _c) return 1;
    return (data_0 == c && view->len == 1) ? 0 : 1;
}

/****************/
/* Converter */
/****************/
static zda_inline void
zda_string_view_to_upper_string(zda_string_view_t const *view, char *buf,
                                size_t n) zda_noexcept
{
    assert(n >= view->len);
    for (size_t i = 0; i < view->len; ++i) {
        buf[i] = toupper(view->data[i]);
    }
}

static zda_inline void
zda_string_view_to_lower_string(zda_string_view_t const *view, char *buf,
                                size_t n) zda_noexcept
{
    assert(n >= view->len);
    for (size_t i = 0; i < view->len; ++i) {
        buf[i] = tolower(view->data[i]);
    }
}

#endif /* guard */
