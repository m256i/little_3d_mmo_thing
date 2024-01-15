#pragma once

#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <new>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <functional>
#include "headers/logging/easylogging++.h"

using b8 = char;
using b0 = bool;
using u0 = void;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;
using f65 = long double;

using usize   = std::size_t;
using uptr    = std::uintptr_t;
using ptrdiff = std::ptrdiff_t;

using i4x32 = __m128i; // 128-bit wide vector used to store integers (signed and unsigned)
using f4x32 = __m128;  // 128-bit wide vector used to store single presicion floats
using f2x64 = __m128d; // 128-bit wide vector used to store double presicion floats

using i8x32 = __m256i; // 256-bit wide vector used to store integers (signed and unsigned)
using f8x32 = __m256;  // 256-bit wide vector used to store single presicion floats
using f4x64 = __m256d; // 256-bit wide vector used to store double presicion floats

#if 0
#define Pure_Global_State __declspec(noalias)
#define Const_Global_State __declspec(noalias)
#define No_Alias_Return __declspec((restrict))
#else
#define Pure_Global_State __attribute__((pure))
#define Const_Global_State __attribute__((const))
#define No_Alias_Return
#endif

template <typename TToType, typename TType>
inline TToType*
mafia(TType* const _ptr)
{
  return (TToType*)(u0*)std::launder(_ptr);
}

template <typename TToType, typename TType>
inline TToType&
ref_mafia(TType* const _ptr)
{
  return (*(TToType*)(u0*)std::launder(_ptr));
}

template <usize TIndex, typename... TTypes>
using nth_type = std::tuple_element_t<TIndex, std::tuple<TTypes...>>;

template <usize TOffset, typename TTuple, usize... TIndices>
decltype(auto)
nth_type_v(std::index_sequence<TIndices...> s)
{
  return std::tuple<std::tuple_element_t<TIndices + TOffset, TTuple>...>{};
}

template <usize TStart, usize TEnd, typename... TTypes>
using type_subrange = decltype(nth_type_v<TStart, std::tuple<TTypes...>>(std::make_index_sequence<TEnd - TStart>{}));

template <typename TType, typename TFunc, std::size_t... TIndex>
constexpr void
visit_impl(TType& _tuple, const size_t _index, TFunc _func, std::index_sequence<TIndex...>)
{
  ((TIndex == _index ? _func(std::get<TIndex>(_tuple)) : void()), ...);
}

template <typename TFunc, typename... TTypes, typename _Indices = std::make_index_sequence<sizeof...(TTypes)>>
constexpr void
visit_at(std::tuple<TTypes...>& _tuple, const size_t _index, TFunc _func)
{
  visit_impl(_tuple, _index, _func, _Indices{});
}

template <typename TFunc, typename... TTypes, typename _Indices = std::make_index_sequence<sizeof...(TTypes)>>
constexpr void
visit_at(const std::tuple<TTypes...>& _tuple, const size_t _index, TFunc _func)
{
  visit_impl(_tuple, _index, _func, _Indices{});
}

/*
visit_at (TUPLE, INDEX,
                   [&] (auto &_arg) {
                     using type = std::decay_t<decltype (_arg)>;
                    ...
                   });
*/

inline auto
mmin(const auto& _a, const auto& _b)
{
  return _a < _b ? _a : _b;
}

inline auto
mmax(const auto& _a, const auto& _b)
{
  return _a > _b ? _a : _b;
}

#ifdef min
#undef min
#else
#ifdef max
#undef max
#endif
#endif

auto
enumerate(auto& _container)
    -> std::vector<std::pair<const std::size_t, std::reference_wrapper<typename std::remove_reference_t<decltype(_container)>::value_type>>>
{
  std::vector<std::pair<const std::size_t, std::reference_wrapper<typename std::remove_reference_t<decltype(_container)>::value_type>>>
      out{};

  std::size_t counter{0};
  for (auto& ite : _container)
  {
    out.emplace_back(counter++, std::ref(ite));
  }
  return out;
}

struct color_t
{
  u8 r, g, b, a;

  color_t(u8 _r, u8 _g, u8 _b, u8 _a) : r(_r), g(_g), b(_b), a(_a) {}
  color_t(u32 _col) : r(_col >> 24), g((_col >> 16) & 0xff), b((_col >> 8) & 0xff), a((_col) & 0xff) {}

  u32
  as_u32()
  {
    return (u32)((u32)r << 24 | (u32)g << 16 | (u32)b << 8 | (u32)a);
  }
};