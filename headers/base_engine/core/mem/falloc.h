#pragma once

#include "../../../../common.h"

#include <array>
#include <future>
#include <memory>
#include <mutex>
#include <new>
#include <immintrin.h> // prefetching
#include <pplinterface.h>
#include <type_traits>
#include <coroutine>
#include <atomic>

/* TODO: bad bad wrong bad fix this */
static constexpr usize cache_line_size = 64;

/* basic memory pool allocator */
struct falloc_t
{
  std::vector<u8> buffer{};
  usize index{};

  falloc_t() = default;

  falloc_t(usize _size)
  {
    buffer.resize(_size);
  }

  u0 resize(usize _new_size)
  {
    buffer.resize(_new_size);
  }

  usize required_space(usize _size, usize _min_alignment)
  {
    usize aligned = _min_alignment;
    while(aligned < _size)
    {
      aligned += _min_alignment;
    }
    return aligned;
  }

  [[nodiscard]] u0* alloc(usize _size, usize _alignment,  usize _count = 1)
  {
    /* make sure that the ptr is aligned properly */
    while (((uptr)buffer.data() + index) % _alignment)
    {
      ++index;
    }
    usize current_index = index;
    if(_count > 1)
    {
      index += required_space(_size, _alignment) * _count;
    }
    else 
    {
      index += _size;
    }
    return (u0*)std::launder(std::addressof(buffer[current_index]));
  }

  [[nodiscard]] u0* prefetched_alloc(usize _size, usize _alignment,  usize _count = 1)
  {
    u0* ptr = alloc(_size, _alignment, _count);
    for (usize i = 0; i != required_space(_size, _alignment) * _count; i+= cache_line_size)
    {
      _mm_prefetch((char*)(u0*)((uptr)ptr + i), _MM_HINT_T0);
    }
    return ptr;
  }

  u0 flush()
  {
    index = 0;
  }
};

/* coroutine based threadsafe allocator */
struct ts_falloc
{
  falloc_t base_allocator{};
  std::mutex mut;

  ts_falloc() = default;

  u0 resize(usize _new_size)
  {
    std::lock_guard l{mut};
    base_allocator.resize(_new_size);
    return;
  }

};

