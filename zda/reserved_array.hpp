#ifndef _ZDA_RESERVED_ARRAY_H_
#define _ZDA_RESERVED_ARRAY_H_

#ifdef _DEBUG_RESERVED_ARRAY_
#  include <iostream>
#  include <stdio.h>

#  define DLOG(...) printf(__VA_ARGS__);
#else
#  define DLOG(...)
#endif

#include <memory>
#include <utility>
#include <assert.h>

#include "zda/zstl/iterator.h"
#include "zda/zstl/type_traits.h"
#include "zda/zstl/uninitialized.h"

#include "zda/mem/libc_allocator_with_realloc.h"

namespace zda {

//************************************************************
// 对于以下类型进行reallocate
// 1) trivial type
// 2) non-trivial class type but with nothrow default constructor
//    (since reallocate() cannot ensure exception-safe)
//    and static class variable "can_reallocate = true"
//************************************************************
template <typename T, typename = void>
struct has_nontype_member_can_reallocate_with_true : std::false_type {};

template <typename T>
struct has_nontype_member_can_reallocate_with_true<T, zstl::void_t<decltype(&T::can_reallocate)>>
  : zstl::bool_constant<T::can_reallocate> {};

template <typename T>
struct can_reallocate
  : zstl::disjunction<
        std::is_trivial<T>,
        zstl::conjunction<
            has_nontype_member_can_reallocate_with_true<T>,
            std::is_nothrow_default_constructible<T>>> {};

// template<typename T, typename HasReallocate=std::true_type>
// struct can_reallocate : std::is_trivial<T> {};

// template<typename T>
// struct can_reallocate<T, typename has_nontype_member_can_reallocate<T>::type>
//   : zstl::disjunction<
//       std::is_trivial<T>,
//       zstl::bool_constant<T::can_reallocate>> {};

// template<typename T>
// struct can_reallocate <T,
// zstl::void_t<zstl::bool_constant<T::can_reallocate>>>
//   : zstl::disjunction<
//       std::is_trivial<T>,
//       zstl::bool_constant<T::can_reallocate>> {};

/**
 * \brief Like std::vector<T> but there is no capacity concept
 *
 * 命名由来：
 * 首先，std::vector<>的名字是命名失误且无法修正，只能沿用，硬要取个与array区别的名字的话，
 * dynamic_array或scalable_array更为合适，直白并且体现其特征。
 *
 * ReservedArray的命名也体现了其特征：其内容首先进行预分配，然后读取或写入其中的内容。
 * 该容器不支持append，即push/emplace_back等，自然也不支持prepend,即push/emplace_front()等，
 * 故不需要capacity数据成员（或说capacity == size)
 * 支持扩展(Grow)和收缩(Shrink)。
 *
 * 换言之，ReservedArray只是个默认初始化的内存区域。
 *
 * 应用场景(e.g.)：
 * 1) hashtable
 * 2) continuous buffer
 */
template <typename T, typename Alloc = LibcAllocatorWithRealloc<T>>
class ReservedArray : protected Alloc {
  using AllocTraits = std::allocator_traits<Alloc>;

  // static_assert(
  //     std::is_default_constructible<T>::value,
  //     "The T(Value) type must be default constructible"
  // );
  // static_assert(
  //     zstl::disjunction<std::is_move_constructible<T>, std::is_copy_constructible<T>>::value,
  //     "The T(Value) type must be move/copy constructible"
  // );

 public:
  using value_type      = T;
  using reference       = T &;
  using const_reference = T const &;
  using pointer         = T *;
  using const_pointer   = T const *;
  using size_type       = size_t;
  using iterator        = pointer;
  using const_iterator  = const_pointer;

  ReservedArray()
    : data_(nullptr)
    , end_(data_)
  {
  }

  explicit ReservedArray(size_type n)
    : data_(AllocTraits::allocate(*this, n))
    , end_(data_ + n)
  {
    if (data_ == NULL) {
      throw std::bad_alloc{};
    }

    // try {
    //   zstl::UninitializedDefaultConstruct(data_, end_);
    // }
    // catch (...) {
    //   AllocTraits::deallocate(*this, data_, n);
    //   data_ = end_ = nullptr;
    //   throw;
    // }
  }

  ReservedArray(ReservedArray const &other) = delete;

  /**
   * @brief
   *
   * @param other
   * @param n Initialized elements count of \p other
   */
  void InitFrom(ReservedArray const &other, size_t n)
  {
    data_ = (AllocTraits::allocate(*this, other.size()));
    end_  = (data_ + other.size());
    try {
      std::uninitialized_copy(other.begin(), n, data_);
    }
    catch (...) {
      AllocTraits::deallocate(*this, data_, other.size());
      data_ = end_ = nullptr;
      throw;
    }
  }

  ReservedArray &operator=(ReservedArray const &other) = delete;

  void CopyFrom(ReservedArray const &other, size_t n, size_t m)
  {
    // 为了保证exception-safe：
    // 对于throw拷贝赋值运算符的类类型分配新内存区域进行copy
    // 而对no throw的直接复用原有内存区域copy
    if (&other != this) {
      CopyAssignmentImpl<value_type>(other);
    }
  }

  ReservedArray(ReservedArray &&other) noexcept
    : data_(other.data_)
    , end_(other.end_)
  {
    other.data_ = other.end_ = nullptr;
  }

  ReservedArray &operator=(ReservedArray &&other) noexcept
  {
    this->swap(other);
    return *this;
  }

  ~ReservedArray() noexcept
  {
    zstl::DestroyRange(data_, end_);
    AllocTraits::deallocate(*this, data_, size());
  }

  void Grow(size_type n, size_type init_n)
  {
    if (n <= GetSize()) {
      return;
    }

    GrowNoCheck(n, init_n);
  }

  void GrowNoCheck(size_type n, size_type init_n) { Reallocate<value_type>(n, init_n); }

  void Shrink(size_type n, size_type init_n)
  {
    if (n >= GetSize()) {
      return;
    }

    ShrinkNoCheck(n, init_n);
  }

  void ShrinkNoCheck(size_type n, size_type init_n) { Shrink_impl<value_type>(n, init_n); }

  size_type GetSize() const noexcept { return end_ - data_; }
  size_type size() const noexcept { return end_ - data_; }
  bool      empty() const noexcept { return data_ == end_; }

  reference operator[](size_type i) noexcept
  {
    assert(i < size());

    return data_[i];
  }

  const_reference operator[](size_type i) const noexcept
  {
    assert(i < size());
    return data_[i];
  }

  iterator       begin() noexcept { return data_; }
  iterator       end() noexcept { return end_; }
  const_iterator begin() const noexcept { return data_; }
  const_iterator end() const noexcept { return end_; }
  const_iterator cbegin() const noexcept { return data_; }
  const_iterator cend() const noexcept { return end_; }

  pointer       data() noexcept { return data_; }
  const_pointer data() const noexcept { return data_; }

  void swap(ReservedArray &other) noexcept
  {
    std::swap(data_, other.data_);
    std::swap(end_, other.end_);
  }

 private:
  template <typename U, zstl::enable_if_t<can_reallocate<U>::value, int> = 0>
  void Reallocate(size_type n, size_type init_n)
  {
    // Failed to call the reallocate(),
    // the old memory block is not freed
    DLOG("size(before realloc): %zu\n", size());
    DLOG("%p\n", this);
    auto tmp = this->reallocate(data_, n);

    if (tmp == NULL) {
      throw std::bad_alloc{};
    }

    // 为了支持包含can_reallocate = true的non-trivial类类型也能进行reallocate
    // 对扩展的内存区域进行默认初始化

    // reallocate无法保证异常安全
    // 因为data_的内存区域可能已被释放
    // DLOG("default consturct [%zu, %zu)\n", size(), n);
    // zstl::UninitializedDefaultConstruct(tmp + size(), tmp + n);

    data_ = tmp;
    end_  = data_ + n;
  }

  template <typename U, zstl::enable_if_t<!can_reallocate<U>::value, char> = 0>
  void Reallocate(size_type n, size_type init_n)
  {
    // To ensure exception-safe,
    // set data_ and end_ at last
    auto new_data = AllocTraits::allocate(*this, n);
    if (new_data == NULL) {
      throw std::bad_alloc{};
    }

    try {
      zstl::UninitializedMoveIfNoexcept(data_, data_ + init_n, new_data);
      // new_end = zstl::UninitializedDefaultConstruct(new_end, new_data + n);
    }
    catch (...) {
      AllocTraits::deallocate(*this, new_data, init_n);
      throw;
    }

    AllocTraits::deallocate(*this, data_, GetSize());
    data_ = new_data;
    end_  = new_data + n;
  }

  template <typename U, zstl::enable_if_t<can_reallocate<U>::value, char> = 0>
  void Shrink_impl(size_type n, size_t init_n)
  {
    // destroy一般来说是no throw的
    DLOG("destroy: [%zu, %zu)\n", n, size());
    /* For trivial type, this do nothing */
    zstl::DestroyRange(data_ + n, end_);

    auto tmp = this->reallocate(data_, n);

    if (tmp == NULL && n != 0) {
      throw std::bad_alloc{};
    }

    if (n == 0) {
      data_ = end_ = nullptr;
    } else {
      data_ = tmp;
      end_  = data_ + n;
    }
  }

  template <typename U, zstl::enable_if_t<!can_reallocate<U>::value, int> = 0>
  void Shrink_impl(size_type n, size_t init_n)
  {
    assert(init_n <= size());
    auto new_data = AllocTraits::allocate(*this, n);
    if (new_data == NULL) {
      throw std::bad_alloc{};
    }

    auto move_n = (init_n > n ? n : init_n);

    try {
      zstl::UninitializedMoveIfNoexcept(data_, data_ + move_n, new_data);
    }
    catch (...) {
      AllocTraits::deallocate(*this, new_data, move_n);
      throw;
    }

    for (auto beg = data_ + move_n; beg != end_; ++beg) {
      AllocTraits::destroy(*this, beg);
    }

    AllocTraits::deallocate(*this, data_, GetSize());

    data_ = new_data;
    end_  = new_data + n;
  }

  template <typename U>
  using cond = zstl::conjunction<
      can_reallocate<U>,
      std::is_nothrow_copy_assignable<U>,
      std::is_nothrow_constructible<U>>;

  template <typename U, typename = zstl::enable_if_t<cond<U>::value>>
  void CopyAssignmentImpl(ReservedArray const &other, size_t n, size_t m)
  {
    if (m > n) {
      /* Just store the initialized data */
      auto tmp = this->reallocate(data_, m);

      if (tmp == NULL) {
        throw std::bad_alloc{};
      }
      auto mid = other.begin() + n;
      std::copy(other.begin(), mid, tmp);
      std::uninitialized_copy(mid, other.begin() + m, tmp + n);
      data_ = tmp;
    } else {
      std::copy(other.begin(), other.begin() + m, data_);
      zstl::DestroyRange(data_ + m, data_ + n);
    }

    end_ = data_ + m;
  }

  template <typename U, zstl::enable_if_t<!cond<U>::value, int> = 0>
  void CopyAssignmentImpl(ReservedArray const &other, size_t n, size_t m)
  {
    if (n < m) {
      auto data = AllocTraits::allocate(*this, m);

      if (data == NULL) {
        throw std::bad_alloc{};
      }

      std::copy(other.begin(), other.begin() + n, data);
      try {
        std::uninitialized_copy(other.begin() + n, other.begin() + m, data + n);
      }
      catch (...) {
        AllocTraits::deallocate(*this, data, other.size());
        throw;
      }

      zstl::DestroyRange(data_, end_);
      AllocTraits::deallocate(*this, data_, size());

      data_ = data;
    } else {
      std::copy(other.begin(), other.begin() + m, data_);
      zstl::DestroyRange(data_ + m, data_ + n);
    }
    end_ = data_ + m;
  }

  T *data_;
  T *end_;
};

} // namespace zda

namespace std {

template <typename T>
inline void swap(zda::ReservedArray<T> &x, zda::ReservedArray<T> &y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

} // namespace std

#endif // Header guard
