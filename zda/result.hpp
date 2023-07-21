#ifndef _ZDA_RESULT_HPP__
#define _ZDA_RESULT_HPP__

#include <utility>
#include "zda/util/assert.h"

namespace zda {

template <typename T>
struct Result {
public:
    Result(T const &obj) 
        : obj_(obj)
        , is_obj_(true)
    {
    }

    Result(T &&obj) noexcept
        : obj_(std::move(obj))
        , is_obj_(true)
    {
    }

    Result(bool ok) noexcept
        : is_obj_(false)
    {
    }

    ~Result() noexcept
    {
        if (is_obj_) obj_.~T();
    }

    Result(Result const &) = delete;

    Result(Result &&rhs) noexcept
        : is_obj_(rhs.is_obj_)
    {
        if (is_obj_) new (this) Result(std::move(rhs.obj_));
    }

    Result &operator=(Result const &) = delete;
    Result &operator=(Result &&rhs) noexcept
    {
        std::swap(is_obj_, rhs.is_obj_);
        if (is_obj_) {
            obj_ = std::move(rhs.obj_);
        }
        return *this;
    }

    T &operator *() noexcept { zda_assert(is_obj_, "The Result<T> don't hold a valid object"); return obj_; }
    T const &operator*() const noexcept { return *((Result*)this); }
    T *operator->() noexcept { zda_assert(is_obj_, "The Result<T> don't hold a valid object"); return &obj_; }
    T const *operator->() const noexcept { return *((Result*)this); }

    operator bool() const noexcept { return is_obj_; }

    template <typename ...Args>
    void inplace_create(Args &&...args)
    {
        new (&obj_) T(std::forward<Args>(args)...);
        is_obj_ = true;
    }
private:
    /* Avoid initialization when `is_obj_` is false */
    union {
        T obj_;
    };
    bool is_obj_;
};

#define RES_NULL false

}

#endif