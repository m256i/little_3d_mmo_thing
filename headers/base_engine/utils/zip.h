#pragma once

#include <common.h>
namespace utils
{

namespace detail
{

template <typename T>
class zip_index
{
public:
  zip_index(T& container) : container_(container) {}

  auto
  begin()
  {
    return zip_iterator(0, container_.begin());
  }

  auto
  end()
  {
    return zip_iterator(container_.size(), container_.end());
  }

private:
  T& container_;

  class zip_iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::pair<std::size_t, typename T::value_type&>;
    using pointer           = value_type*;
    using reference         = value_type&;

    zip_iterator(std::size_t index, typename T::iterator iter) : index_(index), iter_(iter), pair_(index, *iter) {}

    reference
    operator*()
    {
      pair_.first  = index_;
      pair_.second = *iter_;
      return pair_;
    }

    zip_iterator&
    operator++()
    {
      ++index_;
      ++iter_;
      return *this;
    }

    bool
    operator!=(const zip_iterator& other) const
    {
      return iter_ != other.iter_;
    }

  private:
    std::size_t index_;
    typename T::iterator iter_;
    value_type pair_;
  };
};
} // namespace detail

template <typename T>
detail::zip_index<T>
zip(T& container)
{
  return detail::zip_index<T>(container);
}

} // namespace utils