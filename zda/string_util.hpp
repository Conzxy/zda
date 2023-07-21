#ifndef _ZDA_STRING_UTIL_HPP__
#define _ZDA_STRING_UTIL_HPP__

#include <string>

#include "zda/string_view.hpp"
#include "zda/util/macro.h"

#if __cplusplus >= 201703L 
#  include <string_view>
#endif

namespace zda {
namespace detail {

/*
 * The following functions are deprecated
 */
zda_inline size_t _str_len(std::string const &str) zda_noexcept
{
  return str.size();
}

zda_inline size_t _str_len(char const *str) zda_noexcept
{
  return strlen(str);
}

zda_inline size_t _str_len(zda::StringView const &str) zda_noexcept
{
  return str.size();
}

zda_inline size_t _str_len(char) zda_noexcept
{
  return 1;
}

template <typename S, typename... Args>
zda_inline size_t _str_size_impl(S const &head, Args &&... strs) zda_noexcept
{
  return _str_len(head) + _str_size_impl(strs...);
}

template <typename S>
zda_inline size_t _str_size_impl(S const &head) zda_noexcept
{
  return _str_len(head);
}

template <typename... Args>
zda_inline size_t str_size(Args &&... args)
{
  return _str_size_impl(args...);
}

/*
 * 注意，这里必须前向声明，不然递归实例化时，实例化点可能看不到在下面定义的其他str_append_impl()，因此重载决议并不会考虑它们
 * 因此诸如
 * `auto s = str_cat("A", std::string("A"), std::string_view("A"),
 * zda::StringView("A"))`就会报错
 */
template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index,
                    std::string const &head, Args &&... strs);

template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index,
                    zda::StringView const &head, Args &&... strs);

template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index,
                    char const *head, Args &&... strs);

template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index, char head,
                    Args &&... args);

#ifdef CXX_STANDARD_17
template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index,
                    std::string_view const &head, Args &&... strs);
#endif

/*
 * 利用模板递归实例化在编译时展开模板，从而避免了运行时循环开销
 * 由于递归的特性，可以在最后的出口函数进行预分配，
 * 并从尾部开始拷贝最后一个字符串
 *
 * 为兼容C-style，std::string，zda::StringView(zda)，
 * std::string_view(c++17)，char重载了个版本
 *
 * char不能算作长度为1的char*，因为不一定有终止符
 */
#define STRAPPEND_STL_STYLE                                                    \
  do {                                                                         \
    size += head.size();                                                       \
    str_append_impl(str, size, index, strs...);                                 \
    assert(index >= head.size());                                              \
    index -= head.size();                                                      \
    memcpy(&str[index], head.data(), head.size());                             \
  } while (0)

template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index,
                    std::string const &head, Args &&... strs)
{
  STRAPPEND_STL_STYLE;
}

template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index,
                    zda::StringView const &head, Args &&... strs)
{
  STRAPPEND_STL_STYLE;
}

template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index, char head,
                    Args &&... strs)
{
  size += 1;
  str_append_impl(str, size, index, strs...);
  assert(index >= 1);
  index -= 1;
  memcpy(&str[index], &head, 1);
}

template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index,
                    char const *head, Args &&... strs)
{
  auto len = strlen(head);
  size += len;
  str_append_impl(str, size, index, strs...);
  assert(index >= len);
  index -= len;
  memcpy(&str[index], head, len);
}

#ifdef CXX_STANDARD_17
template <typename... Args>
void str_append_impl(std::string &str, size_t &size, size_t &index,
                    std::string_view const &head, Args &&... strs)
{
  STRAPPEND_STL_STYLE;
}
#endif

#define STRAPPEND_STL_STYLE_EXIT                                               \
  do {                                                                         \
    size += head.size();                                                       \
    str.resize(size);                                                          \
    index = size - head.size();                                                \
    memcpy(&str[index], head.data(), head.size());                             \
  } while (0)

template <>
zda_inline void str_append_impl<>(std::string &str, size_t &size,
                                   size_t &index, zda::StringView const &head)
{
  STRAPPEND_STL_STYLE_EXIT;
}

template <>
zda_inline void str_append_impl<>(std::string &str, size_t &size,
                                   size_t &index, std::string const &head)
{
  STRAPPEND_STL_STYLE_EXIT;
}

template <>
zda_inline void str_append_impl<>(std::string &str, size_t &size,
                                   size_t &index, char const *head)
{
  auto len = strlen(head);
  size += len;
  str.resize(size);
  index = size - len;
  memcpy(&str[index], head, len);
}

template <>
zda_inline void str_append_impl<>(std::string &str, size_t &size,
                                   size_t &index, char head)
{
  size += 1;
  str.resize(size);
  index = size - 1;
  memcpy(&str[index], &head, 1);
}

#ifdef CXX_STANDARD_17
template <>
zda_inline void str_append_impl<>(std::string &str, size_t &size,
                                   size_t &index, std::string_view const &head)
{
  STRAPPEND_STL_STYLE_EXIT;
}
#endif

} // namespace detail

/**
 * \brief Append strings efficiently than str.append() with loop or sequence
 *
 * The function will preallocate the space and append them to the \p str,
 * and expand them in compile time, so there is no runtime overhead.
 * \note
 *  The performance is better than the absl::str_append()
 */
template <typename... Args>
zda_inline void str_append(std::string &str, Args &&... strs)
{
  size_t size = str.size();
  size_t index = 0; // Any value is OK
  detail::str_append_impl(str, size, index, strs...);
}

/**
 * \brief Concatenate the strings to a single string efficiently
 *
 * Trivially, a rookie may concatenate the string in the following:
 * ```cpp
 * std::string str = s1 + s2 + s3;
 * // Worse
 * str = str + s1 + s2 + s3;
 * ```
 * This approach will create many temporary object, therefore, to avoid it,
 * maybe:
 * ```cpp
 * std::string str;
 * str += s1;
 * str += s2;
 * str += s3;
 * ```
 * This is OK, but the memory need reallocate multiple times.
 * The better approach is preallocate throught the reserve() method:
 * ```cpp
 * std::string str;
 * str.reserve(s1.size() + s2.size() + s3.size())
 * str += ...
 * ```
 * But the progress is cumbersome, and this function will do it.
 * \note
 *  In fact, this a wrapper of the str_append(empty-string, strs...)
 */
template <typename... Args>
zda_inline std::string str_cat(Args &&... strs)
{
  std::string ret;
  str_append(ret, strs...);
  return ret;
}

}

#endif