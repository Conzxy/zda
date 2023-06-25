#ifndef _ZDA_DARRAY_HPP__
#define _ZDA_DARRAY_HPP__

#include <stdexcept>
#include "zda/reserved_array.hpp"
#include "zda/util/macro.h"
#include "zda/zstl/uninitialized.h"

namespace zda {

template <typename T, typename Alloc = LibcAllocatorWithRealloc<T>>
class Darray : protected Alloc {
  using AllocTraits = std::allocator_traits<Alloc>;

 public:
  using value_type      = T;
  using reference       = T &;
  using const_reference = T const &;
  using pointer         = T *;
  using const_pointer   = T const *;
  using size_type       = size_t;
  using iterator        = pointer;
  using const_iterator  = const_pointer;

  Darray();
  Darray(Darray const &);
  Darray(Darray &&) noexcept;
  Darray &operator=(Darray const &rhs);
  Darray &operator=(Darray &&rhs) noexcept;
  ~Darray() noexcept;

  template <typename... Args>
  void Add(Args &&...args);

  void Remove();

  void Clear()
  {
    zstl::DestroyRange(begin(), end());
    tail_ = begin();
  }

  void Resize(size_t n);
  void Reserve(size_t n);
  void Shrink(size_t n);
  void ShrinkToFit() { Shrink(size()); }

  size_t GetSize() const noexcept { return tail_ - region_.begin(); }
  size_t GetCapacity() const noexcept { return capacity(); }
  size_t size() const noexcept { return GetSize(); }
  size_t capacity() const noexcept { return region_.size(); }
  bool   IsEmpty() const noexcept { return tail_ == begin(); }

  iterator       begin() noexcept { return region_.begin(); }
  iterator       end() noexcept { return tail_; }
  const_iterator begin() const noexcept { return region_.begin(); }
  const_iterator end() const noexcept { return tail_; }
  const_iterator cbegin() const noexcept { return region_.begin(); }
  const_iterator cend() const noexcept { return tail_; }

  T &operator[](size_t i) noexcept
  {
    assert(i < size());
    return region_[i];
  }
  T const &operator[](size_t i) const noexcept
  {
    assert(i < size());
    return region_[i];
  }

  T &At(size_t i) noexcept
  {
    if (i >= size()) throw std::logic_error("Darray: out of index");
    return region_[i];
  }
  T const &At(size_t i) const noexcept { return (Darray *)this->at(i); }

  T &GetBack() noexcept
  {
    assert(!IsEmpty());
    return *(tail_ - 1);
  }
  T const &GetBack() const noexcept
  {
    assert(!IsEmpty());
    return *(tail_ - 1);
  }

  T &GetFront() noexcept
  {
    assert(!IsEmpty());
    return *(begin());
  }
  T const &GetFront() const noexcept
  {
    assert(!IsEmpty());
    return *(begin());
  }

 private:
  void ReservePushSpace(size_t n);

  ReservedArray<T, Alloc> region_;
  T                      *tail_;
};

#define _DARRAY_TEMPLATE_LIST  template <typename T, typename A>
#define _DARRAY_TEMPLATE_CLASS Darray<T, A>

_DARRAY_TEMPLATE_LIST
zda_inline _DARRAY_TEMPLATE_CLASS::Darray()
  : region_()
{
  tail_ = nullptr;
}

_DARRAY_TEMPLATE_LIST
zda_inline _DARRAY_TEMPLATE_CLASS::~Darray() noexcept {}

_DARRAY_TEMPLATE_LIST
zda_inline _DARRAY_TEMPLATE_CLASS::Darray(Darray const &rhs)
{
  region_.InitFrom(rhs.region_, rhs.size());
  tail_ = begin() + rhs.size();
}

_DARRAY_TEMPLATE_LIST
zda_inline _DARRAY_TEMPLATE_CLASS::Darray(Darray &&rhs) noexcept
  : region_(std::move(rhs.region_))
{
  tail_ = begin() + rhs.size();
}

_DARRAY_TEMPLATE_LIST
zda_inline auto _DARRAY_TEMPLATE_CLASS::operator=(Darray const &rhs) -> Darray &
{
  region_.CopyFrom(rhs.region_, size(), rhs.size());
  tail_ = begin() + rhs.size();
  return *this;
}

_DARRAY_TEMPLATE_LIST
zda_inline auto _DARRAY_TEMPLATE_CLASS::operator=(Darray &&rhs) noexcept -> Darray &
{
  region_ = (std::move(rhs.region_));
  tail_   = region_.begin() + rhs.size();
}

_DARRAY_TEMPLATE_LIST
template <typename... Args>
zda_inline void _DARRAY_TEMPLATE_CLASS::Add(Args &&...args)
{
  ReservePushSpace(1);
  AllocTraits::construct(*this, tail_, std::forward<Args>(args)...);
  ++tail_;
}

_DARRAY_TEMPLATE_LIST
zda_inline void _DARRAY_TEMPLATE_CLASS::Remove()
{
  AllocTraits::destroy(*this, tail_);
  --tail_;
}

_DARRAY_TEMPLATE_LIST
zda_inline void _DARRAY_TEMPLATE_CLASS::Resize(size_t n)
{
  if (n <= size()) {
    zstl::DestroyRange(begin() + n, tail_);
    tail_ = begin() + n;
  } else {
    Reserve(n);
  }
}

_DARRAY_TEMPLATE_LIST
zda_inline void _DARRAY_TEMPLATE_CLASS::Reserve(size_t n)
{
  if (n > region_.size()) {
    const auto old_sz = size();
    region_.GrowNoCheck(n, size());
    tail_ = begin() + old_sz;
  }
}

_DARRAY_TEMPLATE_LIST
zda_inline void _DARRAY_TEMPLATE_CLASS::Shrink(size_t n)
{
  if (n < region_.size()) {
    const auto old_sz = size();
    region_.Shrink(n, size());
    tail_ = begin() ? begin() + old_sz : begin();
  }
}

/* Private Helper */
_DARRAY_TEMPLATE_LIST
zda_inline void _DARRAY_TEMPLATE_CLASS::ReservePushSpace(size_t n)
{
  assert(n > 0);
  if (n <= (capacity() - size())) return;
  Reserve(n > size() ? (n + size()) : (size() << 1));
}

} // namespace zda

#endif /* Header guard */
