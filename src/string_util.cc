#include "zda/string_util.hpp"
#include "zda/string_view.hpp"

#include <cstdarg>
#include <cstring>

using namespace zda;

size_t zda::vbuf_catf(char *buf, size_t n, char const *fmt,
                      va_list slist) zda_noexcept
{
    char const *p_c = fmt;
    bool present_occur = false;
    size_t ret = 0;

    for (; *p_c; ++p_c) {
        if (present_occur) {
            switch (*p_c) {
                case '%':
                    ++ret;
                    if (buf && ret < n) {
                        *buf = '%';
                        ++buf;
                    }
                    break;
                case 'c': {
                    ++ret;
                    const char c = (char)va_arg(slist, int);
                    if (buf && ret < n) {
                        *buf = c;
                        ++buf;
                    }
                } break;
                case 's': {
                    char const *s = va_arg(slist, char const *);
                    /*
                                        const auto len = strlen(s);
                                        ret += len;
                                        if (p_str && ret < n) {
                                            memcpy(p_str, s, len);
                                            p_str += len;
                                        }
                    */
                    for (;; ++s) {
                        if (*s) {
                            if (++ret < n) {
                                if (buf) *buf++ = *s;
                            }
                        } else
                            break;
                    }
                } break;
                case 'v': {
                    auto s = va_arg(slist, zda::StringView);
                    ret += s.size();
                    if (buf && ret < n) {
                        memcpy(buf, s.data(), s.size());
                        buf += s.size();
                    }
                } break;
                case 'S': {
                    std::string const *p_s = va_arg(slist, std::string const *);
                    ret += p_s->size();
                    if (buf && ret < n) {
                        memcpy(buf, p_s->data(), p_s->size());
                        buf += p_s->size();
                    }
                } break;
                default:
                    return -1;
            }
            present_occur = false;
        } else {
            if (*p_c == '%') {
                present_occur = true;
            } else {
                ++ret;
                if (buf && ret < n) {
                    *buf++ = *p_c;
                }
            }
        }
    }

    if (buf) *buf = 0;
    return ret;
}

size_t zda::buf_catf(char *buf, size_t n, const char *fmt, ...) noexcept
{
    va_list slist;
    va_start(slist, fmt);
    const auto ret = vbuf_catf(buf, n, fmt, slist);
    va_end(slist);
    return ret;
}

size_t zda::vstr_appendf(std::string *p_str, char const *fmt,
                         va_list slist) zda_noexcept
{
    va_list slist2;
    va_copy(slist2, slist);
    const auto request_len = vbuf_catf(nullptr, 0, fmt, slist2);

    if (request_len == -1) return -1;

    const auto old_size = p_str->size();
    p_str->resize(request_len + old_size);
    vbuf_catf(&(*p_str)[old_size], request_len + 1, fmt, slist);
    va_end(slist2);
    return 0;
    /*    char const *p_c = fmt;
        bool present_occur = false;
        size_t ret = 0;

        for (; *p_c; ++p_c) {
            if (present_occur) {
                switch (*p_c) {
                    case '%':
                        if (p_str)
                            *p_str += '%';
                        else
                            ++ret;
                        break;
                    case 'c': {
                        const char c = (char)va_arg(slist, int);
                        if (p_str)
                            *p_str += c;
                        else
                            ++ret;
                    } break;
                    case 's': {
                        char const *s = va_arg(slist, char const *);
                        if (p_str)
                            p_str->append(s);
                        else
                            ret += strlen(s);
                    } break;
                    case 'v': {
                        auto s = va_arg(slist, zda::StringView);
                        if (p_str)
                            p_str->append(s.data(), s.size());
                        else
                            ret += s.size();
                    } break;
                    case 'S': {
                        std::string const *p_s = va_arg(slist, std::string
       const
       *); if (p_str) p_str->append(*p_s); else ret += p_s->size(); } break;
                    default:
                        return -1;
                }
                present_occur = false;
            } else {
                if (*p_c == '%') {
                    present_occur = true;
                } else {
                    if (p_str) {
                        *p_str += *p_c;
                    } else {
                        ++ret;
                    }
                }
            }
        }
        return ret;*/
}

size_t zda::str_appendf(std::string *p_str, const char *fmt, ...) zda_noexcept
{
    va_list slist;
    va_start(slist, fmt);
    const auto ret = vstr_appendf(p_str, fmt, slist);
    va_end(slist);
    return ret;
}

std::string zda::vstr_catf(char const *fmt, va_list slist) zda_noexcept
{
    std::string ret;
    vstr_appendf(&ret, fmt, slist);
    return ret;
}

std::string zda::str_catf(char const *fmt, ...) zda_noexcept
{
    va_list slist;
    va_start(slist, fmt);
    auto ret = vstr_catf(fmt, slist);
    va_end(slist);
    return ret;
}