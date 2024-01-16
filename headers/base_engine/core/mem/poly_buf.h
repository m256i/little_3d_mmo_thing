#pragma once

#include <new>
#include <vector>

#include "../../../../common.h"

struct poly_buf_t
{
  template <typename TType>
  decltype(auto)
  as(usize index)
  {
    TType* const ptr = std::launder(reinterpret_cast<TType*>(reinterpret_cast<u0*>(&buffer[index])));
    return (*ptr);
  }

  template <typename TType>
  decltype(auto)
  as(usize index) const
  {
    const TType* const ptr = std::launder(reinterpret_cast<const TType*>(reinterpret_cast<const u0*>(&buffer[index])));
    return (*ptr);
  }

  std::vector<u8> buffer;
};