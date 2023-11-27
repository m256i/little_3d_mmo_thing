#pragma once

#include "../../../../common.h"

#include <array>
#include <future>
#include <memory>
#include <mutex>
#include <new>
#include <immintrin.h> // prefetching
#include <type_traits>
#include <coroutine>
#include <atomic>
#include <vector>

/* TODO: bad bad wrong bad fix this */
static constexpr usize cache_line_size = 64;

/* basic memory pool allocator */
struct falloc_t
{
  std::vector<u8> buffer{};
  usize index{};

  inline falloc_t(usize _size) noexcept { buffer.resize(_size); }

  u0
  resize(usize _new_size) noexcept
  {
    buffer.resize(_new_size);
  }

  usize
  required_space(usize _size, usize _min_alignment) noexcept
  {
    usize aligned = _min_alignment;
    while (aligned < _size)
    {
      aligned += _min_alignment;
    }
    return aligned;
  }

  [[nodiscard]] u0*
  alloc(usize _size, usize _alignment = 4, usize _count = 1) noexcept
  {
    /* make sure that the ptr is aligned properly */
    while (((uptr)buffer.data() + index) % _alignment)
    {
      ++index;
    }
    usize current_index = index;
    if (_count > 1)
    {
      index += required_space(_size, _alignment) * _count;
    }
    else
    {
      index += _size;
    }
    return (u0*)std::launder(std::addressof(buffer[current_index]));
  }

  [[nodiscard]] u0*
  prefetched_alloc(usize _size, usize _alignment = 4, usize _count = 1) noexcept
  {
    u0* ptr = alloc(_size, _alignment, _count);
    for (usize i = 0; i != required_space(_size, _alignment) * _count; i += cache_line_size)
    {
      _mm_prefetch((char*)(u0*)((uptr)ptr + i), _MM_HINT_T0);
    }
    return ptr;
  }

  u0
  flush()
  {
    index = 0;
  }
};

/* threadsafe allocator */
// TODO: remove ugly mutex add something more modern/fast
struct ts_falloc
{
  falloc_t base_allocator;
  std::mutex mut{};

  inline ts_falloc(usize _size) noexcept : base_allocator(_size) {}

  u0
  resize(usize _new_size) noexcept
  {
    std::lock_guard l{mut};
    base_allocator.resize(_new_size);
    return;
  }

  [[nodiscard]] u0*
  alloc(usize _size, usize _alignment = 4, usize _count = 1) noexcept
  {
    std::lock_guard l{mut};
    return base_allocator.alloc(_size, _alignment, _count);
  }

  [[nodiscard]] u0*
  prefetched_alloc(usize _size, usize _alignment = 4, usize _count = 1) noexcept
  {
    std::lock_guard l{mut};
    return base_allocator.prefetched_alloc(_size, _alignment, _count);
  }

  inline u0
  flush() noexcept
  {
    std::lock_guard l{mut};
    base_allocator.index = 0;
  }
};
