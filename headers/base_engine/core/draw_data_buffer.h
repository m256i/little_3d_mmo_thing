#pragma once
#include <algorithm>
#include <vector>
#include <ranges>
#include <assert.h>
#include <concepts>
#include <tuple>

#include "../../../common.h"
#include "base_engine/core/draw_data_buffer.h"

template <typename... TTypes>
consteval usize
pack_size()
{
  return (sizeof(TTypes) + ...);
}

template <template <class...> typename, typename...>
struct tuple_pack_size;

template <template <class...> typename TTuple, typename... TTupleTypes>
struct tuple_pack_size : TTuple<TTupleTypes...>
{
  constexpr static usize value = pack_size<TTupleTypes...>();
  static consteval usize
  operator()()
  {
    return value;
  }
};

auto a = tuple_pack_size<std::vector<int>, int>::value;

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
  return (ctr.size * sizeof(decltype(ctr)::value_type));
}

template <typename... TTypes>
struct draw_buffer_t
{
  /* the total size of the parameter pack aka <float, float> = 8, <float, float, std::string> = 40 */
  static constexpr usize pack_size = ::pack_size<TTypes...>();

  inline draw_buffer_t() = default;

  inline draw_buffer_t(const draw_buffer_t& other) : cache(other.cache) {}
  inline draw_buffer_t(draw_buffer_t&& other) : cache(std::move(other.cache)) {}

  inline decltype(auto) operator=(const draw_buffer_t & other) { cache = other.cache; }
  inline decltype(auto) operator=(draw_buffer_t && other) { cache = std::move(other.cache); }

  template <typename TType, usize TSize>
  constexpr draw_buffer_t(const TType (&arr)[TSize], usize count)
  {
    assert((pack_size * count) == TSize);

    cache.resize(TSize);
    std::ranges::copy(arr, cache);
  }

  constexpr draw_buffer_t(std_copyable auto& data, usize count)
  {
    assert((pack_size * count) == total_size(data));

    cache.resize(total_size(data));
    std::ranges::copy(data, cache);
  }

  constexpr draw_buffer_t(std_copyable auto& data)
  {
    assert((pack_size * data.size()) == total_size(data));

    cache.resize(total_size(data));
    std::ranges::copy(data, cache);
  }

  draw_buffer_t(const u8* ptr, usize size, usize count)
  {
    assert((pack_size * count) == size);
    cache.resize(size);
    std::memcpy(cache.data(), ptr, size);
  }

  template <usize TLaneIndex>
  bool
  load_buffer(const std_copyable auto& buffer)
  {
    static_assert(TLaneIndex < sizeof...(TTypes), "lane index out of bounds!");
    using type = nth_type<TLaneIndex, TTypes...>;
    static_assert(sizeof(type) == decltype(buffer)::value_type, "type mismatch on lanes");
    assert(total_size(buffer) == (sizeof(type) * buffer.size()));

    /* grow cache if needed */
    if (cache.size() < pack_size * buffer.size())
    {
      cache.resize(pack_size * buffer.size());
    }

    static constexpr auto mem_offset = tuple_pack_size<type_subrange<TLaneIndex - 1, sizeof...(TTypes), TTypes...>>() - pack_size;

    for (const auto [index, ite] : enumerate(buffer))
    {
      cache[index * pack_size + mem_offset] = ite;
    }

    return true;
  }

  template <usize TLaneIndex>
  bool
  load_buffer(std_copyable auto&& buffer)
  {
    static_assert(TLaneIndex < sizeof...(TTypes), "lane index out of bounds!");
    using type = nth_type<TLaneIndex, TTypes...>;
    static_assert(sizeof(type) == decltype(buffer)::value_type, "type mismatch on lanes");
    assert(total_size(buffer) == (sizeof(type) * buffer.size()));

    /* grow cache if needed */
    if (cache.size() < pack_size * buffer.size())
    {
      cache.resize(pack_size * buffer.size());
    }

    static constexpr auto mem_offset = tuple_pack_size<type_subrange<TLaneIndex - 1, sizeof...(TTypes), TTypes...>>() - pack_size;

    for (const auto [index, ite] : enumerate(buffer))
    {
      cache[index * pack_size + mem_offset] = std::move(ite);
    }

    return true;
  }

  std::vector<u8> cache{};
};