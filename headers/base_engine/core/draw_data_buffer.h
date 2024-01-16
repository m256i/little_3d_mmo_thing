#pragma once
#include <algorithm>
#include <type_traits>
#include <vector>
#include <ranges>
#include <assert.h>
#include <concepts>
#include <tuple>

#include "../../../common.h"
#include "common.h"

template <typename... TTypes>
consteval usize
pack_size()
{
  return (sizeof(TTypes) + ...);
}

template <typename... TTuple>
struct tuple_pack_size;

template <typename... TArgs>
struct tuple_pack_size<std::tuple<TArgs...>>
{
  static constexpr usize value = pack_size<TArgs...>();
};

template <typename TType>
concept std_copyable = requires(TType type) {
                         /* we want to be able to copy and also get size information from type */
                         // std::ranges::copy(TType{}, TType{});
                         typename TType::value_type;
                         type.size();
                         type.data();
                       };

constexpr usize
total_size(const std_copyable auto& ctr)
{
  using ctr_type = std::remove_reference_t<decltype(ctr)>;
  return (ctr.size() * sizeof(typename ctr_type::value_type));
}

template <typename... TTypes>
struct draw_buffer_impl
{
  static_assert((... && std::is_trivially_destructible_v<TTypes>),
                "what are you doing silly? tring to have a non-trivially destructible object in a GPU buffer!");

  template <typename TType>
  decltype(auto)
  cache_as(std::vector<u8>& _buffer, usize _index)
  {
    TType* const ptr = std::launder(reinterpret_cast<TType*>(reinterpret_cast<u0*>(&_buffer[_index])));
    return (*ptr);
  }

  template <typename TType>
  decltype(auto)
  cache_as(const std::vector<u8>& _buffer, usize _index) const
  {
    const TType* const ptr = std::launder(reinterpret_cast<const TType*>(reinterpret_cast<const u0*>(&_buffer[_index])));
    return (*ptr);
  }

  /* the total size of the parameter pack aka <float, float> = 8, <float, float, std::string> = 40 */
  static constexpr usize pack_size = ::pack_size<TTypes...>();

  inline draw_buffer_impl() = default;
  inline draw_buffer_impl(const draw_buffer_impl& other) : cache(other.cache) {}
  inline draw_buffer_impl(draw_buffer_impl&& other) : cache(std::move(other.cache)) {}

  inline decltype(auto)
  operator=(const draw_buffer_impl& other)
  {
    cache = other.cache;
  }

  inline decltype(auto)
  operator=(draw_buffer_impl&& other)
  {
    cache = std::move(other.cache);
  }

  template <typename TType, usize TSize>
  constexpr draw_buffer_impl(const TType (&arr)[TSize], usize count)
  {
    assert((pack_size * count) == TSize);
    cache.resize(TSize);
    std::ranges::copy(arr, cache);
  }

  constexpr draw_buffer_impl(std_copyable auto& data, usize count)
  {
    assert((pack_size * count) == total_size(data));
    cache.resize(total_size(data));
    std::ranges::copy(data, cache);
  }

  constexpr draw_buffer_impl(std_copyable auto& data)
  {
    assert((pack_size * data.size()) == total_size(data));
    cache.resize(total_size(data));
    std::ranges::copy(data, cache);
  }

  draw_buffer_impl(const u8* ptr, usize size, usize count)
  {
    assert((pack_size * count) == size);
    cache.resize(size);
    std::memcpy(cache.data(), ptr, size);
  }

  template <usize TLaneIndex>
  bool
  load_buffer(const std_copyable auto& buffer)
  {
    using buffer_type = std::remove_reference_t<decltype(buffer)>;
    static_assert(TLaneIndex < sizeof...(TTypes), "lane index out of bounds!");

    using type = nth_type<TLaneIndex, TTypes...>;

    static_assert(sizeof(type) == sizeof(typename buffer_type::value_type), "type mismatch on lanes");
    assert(total_size(buffer) == (sizeof(type) * buffer.size()));

    /* grow cache if needed */
    if (cache.size() < pack_size * buffer.size())
    {
      cache.resize(pack_size * buffer.size());
    }

    static constexpr auto mem_offset = tuple_pack_size<type_subrange<TLaneIndex - 1, sizeof...(TTypes), TTypes...>>::value - pack_size;

    for (const auto [index, ite] : enumerate(buffer))
    {
      cache_as<decltype(ite)>(cache, index * pack_size + mem_offset) = ite;
    }

    return true;
  }

  std::vector<u8> cache{};
};

template <ct_string TName, typename TType>
struct buf_elem
{
  /* stupid */
  using _impl_is_buf_elem_TAG = u0;
  /* me when c++ no reflection */

  constexpr static std::string_view name = TName.to_view();
  using type                             = TType;
};

template <typename... TBufferMembers>
  requires requires(TBufferMembers... things) { (... && std::is_same_v<typename TBufferMembers::_impl_is_buf_elem_TAG, u0>); }
struct draw_buf_t
{
  draw_buffer_impl<typename TBufferMembers::type...> buffer;

  template <ct_string TString>
  consteval usize
  index_of_str()
  {
    usize out_index = (usize)-1;
    // clang-format off
    for_constexpr<0ull, sizeof...(TBufferMembers), 1ull>(
      [&](const auto _index) {
        using current_type = nth_type<_index, TBufferMembers...>;
        if constexpr (current_type::name == TString.to_view())
        {
          out_index = _index;
        }
      }
    );
    // clang-format on
    return out_index;
  }

  template <ct_string TBufferName>
  u0
  load(const auto& _new_data)
  {
    static constexpr usize lane_index = index_of_str<TBufferName>();
    static_assert(lane_index != (usize)-1, "typo in ctime-buffer name");

    buffer.load_buffer<lane_index>(_new_data);
  }
};
