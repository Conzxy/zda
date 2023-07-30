#ifndef __ZDA_STIRNG_VIEW_HPP___
#define __ZDA_STIRNG_VIEW_HPP___

#include "zda/string_view.h"

#include <string>

namespace zda {

#define DefStringViewLiteral(var_name, literal)                                \
    StringView var_name(literal, sizeof(literal) - 1)
#define StringViewLiteral(literal) StringView(literal, sizeof(literal) - 1)

class StringView {
   public:
    using size_type = size_t;

    constexpr StringView(char const *data, size_t len) zda_noexcept
    {
        zda_string_view_init(&view_, data, len);
    }

    explicit StringView(char const *c_str) zda_noexcept
    {
        zda_string_view_str_init(&view_, c_str);
    }

    StringView(std::string const &str) zda_noexcept
      : StringView(str.data(), str.size())
    {
    }

    size_type size() const zda_noexcept { return view_.len; }
    void empty() zda_noexcept { zda_string_view_empty(&view_); }
    void is_empty() const zda_noexcept { zda_string_view_is_empty(&view_); }

    char const *data() const zda_noexcept { return view_.data; }
    char const *begin() const zda_noexcept
    {
        return zda_string_view_first(&view_);
    }
    char const *end() const zda_noexcept { return zda_string_view_end(&view_); }

    char operator[](size_t i) const zda_noexcept
    {
        return zda_string_view_at(&view_, i);
    }

    StringView substr(size_t pos, size_t n) const zda_noexcept
    {
        auto view = zda_string_view_substr(&view_, pos, n);
        return StringView(view.data, view.len);
    }

    StringView substr_range(size_t begin, size_t end) const zda_noexcept
    {
        return StringView(zda_string_view_substr_range(&view_, begin, end));
    }

    StringView left(size_t n) const zda_noexcept
    {
        return zda_string_view_left(&view_, n);
    }

    StringView right(size_t n) const zda_noexcept
    {
        return zda_string_view_right(&view_, n);
    }

    void remove_prefix(size_t n) zda_noexcept
    {
        zda_string_view_remove_prefix(&view_, n);
    }

    void remove_suffix(size_t n) zda_noexcept
    {
        zda_string_view_remove_suffix(&view_, n);
    }

    size_t find(StringView str, size_t pos = 0) const zda_noexcept
    {
        return zda_string_view_find(&view_, str.view_, pos);
    }

    size_t find(char c, size_t pos = 0) const zda_noexcept
    {
        return zda_string_view_find_char(&view_, c, pos);
    }

    size_t rfind(StringView str,
                 size_t pos = ZDA_STRING_VIEW_NPOS) const zda_noexcept
    {
        return zda_string_view_rfind(&view_, str.view_, pos);
    }

    size_t rfind(char c, size_t pos = ZDA_STRING_VIEW_NPOS) const zda_noexcept
    {
        return zda_string_view_rfind_char(&view_, c, pos);
    }

    bool contains(StringView str) const zda_noexcept
    {
        return zda_string_view_contains(&view_, str.view_);
    }

    bool contains(char c) const zda_noexcept
    {
        return zda_string_view_contains_char(&view_, c);
    }

    bool starts_with(StringView prefix) const zda_noexcept
    {
        return zda_string_view_starts_with(&view_, prefix.view_);
    }

    bool ends_with(StringView suffix) const zda_noexcept
    {
        return zda_string_view_ends_with(&view_, suffix.view_);
    }

    bool find_first_of(StringView range, size_t pos = 0) const zda_noexcept
    {
        return zda_string_view_find_first_of(&view_, range.view_, pos);
    }

    bool find_first_of(char c, size_t pos = 0) const zda_noexcept
    {
        return zda_string_view_find_first_of_char(&view_, c, pos);
    }

    bool find_last_of(StringView range,
                      size_t pos = ZDA_STRING_VIEW_NPOS) const zda_noexcept
    {
        return zda_string_view_find_last_of(&view_, range.view_, pos);
    }

    bool find_last_of(char c,
                      size_t pos = ZDA_STRING_VIEW_NPOS) const zda_noexcept
    {
        return zda_string_view_find_last_of_char(&view_, c, pos);
    }

    int compare(StringView str) const zda_noexcept
    {
        return zda_string_view_compare(&view_, str.view_);
    }

    int compare(char c) const zda_noexcept
    {
        return zda_string_view_compare_char(&view_, c);
    }

    int case_compare(StringView str) const zda_noexcept
    {
        return zda_string_view_case_compare(&view_, str.view_);
    }

    int case_compare(char c) const zda_noexcept
    {
        return zda_string_view_case_compare_char(&view_, c);
    }

    void to_upper_buffer(char *buf, size_t n) const zda_noexcept
    {
        zda_string_view_to_upper_string(&view_, buf, n);
    }

    void to_lower_buffer(char *buf, size_t n) const zda_noexcept
    {
        zda_string_view_to_lower_string(&view_, buf, n);
    }

    std::string to_upper_string() const zda_noexcept
    {
        std::string ret;
        ret.resize(view_.len);

        zda_string_view_to_upper_string(&view_, &ret[0], ret.size());
        return ret;
    }

    std::string to_lower_string() const zda_noexcept
    {
        std::string ret;
        ret.resize(view_.len);

        zda_string_view_to_lower_string(&view_, &ret[0], ret.size());
        return ret;
    }

    zda_string_view_t *rep() zda_noexcept { return &view_; }
    zda_string_view_t const *rep() const zda_noexcept { return &view_; }

   private:
    StringView(zda_string_view_t view) zda_noexcept : view_(view) {}

    zda_string_view_t view_;
};

zda_inline bool operator<(StringView const &lhs,
                          StringView const &rhs) zda_noexcept
{
    return lhs.compare(rhs) < 0;
}

zda_inline bool operator>=(StringView const &lhs,
                           StringView const &rhs) zda_noexcept
{
    return !(lhs < rhs);
}

zda_inline bool operator>(StringView const &lhs,
                          StringView const &rhs) zda_noexcept
{
    return lhs.compare(rhs) > 0;
}

zda_inline bool operator<=(StringView const &lhs,
                           StringView const &rhs) zda_noexcept
{
    return !(lhs > rhs);
}

zda_inline bool operator==(StringView const &lhs,
                           StringView const &rhs) zda_noexcept
{
    return lhs.compare(rhs) == 0;
}

zda_inline bool operator!=(StringView const &lhs,
                           StringView const &rhs) zda_noexcept
{
    return !(lhs == rhs);
}

namespace literal {
constexpr StringView operator""_sv(char const *str, size_t len) zda_noexcept
{
    return StringView(str, len);
}
} // namespace literal

} // namespace zda

#endif /* guard */
