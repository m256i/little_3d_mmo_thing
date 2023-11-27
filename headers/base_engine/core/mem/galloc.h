#pragma once

#include "../../../../common.h"

#include <array>
#include <memory>
#include <new>

// TODO: very incomplete adding later

/* general purpose slab alloc  */
struct g_alloc_t
{
  template <usize TSize>
  struct sized_block_t
  {
    std::array<u8, TSize> buffer{};

    template <typename TType>
    inline decltype(auto)
    as() noexcept
    {
      return (*((TType*)(u0*)std::launder(buffer.data())));
    }

    template <typename TType>
    inline decltype(auto)
    as() const noexcept
    {
      return (*((const TType*)(u0*)std::launder(buffer.data())));
    }
  };

  template <usize TSize, usize TCount>
  struct slab_page_t
  {
    slab_page_t()
    {
      free_head = buffer[0].template as<sized_block_t<TSize>*>();

      for (usize i = 0; i < TCount - 1; i++)
      {
        buffer.at(i).template as<sized_block_t<TSize>*>() = &(buffer.at(i + 1).template as<sized_block_t<TSize>>());
      }
    }

    std::array<sized_block_t<TSize>, TCount> buffer{};
    sized_block_t<TSize>* free_head{nullptr};
  };

  template <typename TType>
  [[nodiscard]] TType*
  alloc(usize _count) noexcept
  {
  }
};