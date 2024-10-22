#pragma once

#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <new>
#include <string_view>
#include <tuple>
#include <array>
#include <type_traits>
#include <utility>
#include <vector>
#include <functional>
#include "headers/logging/easylogging++.h"

#ifdef _DEBUG
static constexpr bool IS_DEBUG_BUILD = true;
#else
static constexpr bool IS_DEBUG_BUILD = true;
#endif

/*
explictly define functions that are run rarely/once frame as OFFLINE so that we can signalize that this function doesnt
need much optimization
*/
#define OFFLINE
/*
explictly define functions that are run every frame as HOT_PATH to signalize that the function needs to be fast
*/
#define HOT_PATH

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
    out.push_back(std::make_pair(counter++, std::ref(ite)));
  }
  return out;
}

auto
enumerate(const auto& _container)
    -> std::vector<
        std::pair<const std::size_t, std::reference_wrapper<const typename std::remove_reference_t<decltype(_container)>::value_type>>>
{
  std::vector<
      std::pair<const std::size_t, std::reference_wrapper<const typename std::remove_reference_t<decltype(_container)>::value_type>>>
      out{};

  std::size_t counter{0};
  for (const auto& ite : _container)
  {
    out.push_back(std::make_pair(counter++, std::ref(ite)));
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

template <size_t TBegin, size_t TEnd, size_t TStep, typename TLambda>
constexpr void
for_constexpr(TLambda&& _lambda)
{
  if constexpr (TBegin < TEnd)
  {
    _lambda(std::integral_constant<decltype(TBegin), TBegin>());
    for_constexpr<TBegin + TStep, TEnd, TStep>(_lambda);
  }
}

template <usize TArrayLen1, usize TArrayLen2, typename TArrayType>
static constexpr std::array<TArrayType, TArrayLen1 + TArrayLen2 - 1>
concat_arrays(const std::array<TArrayType, TArrayLen1> a, const std::array<TArrayType, TArrayLen2> b)
{
  std::array<TArrayType, TArrayLen1 + TArrayLen2 - 1> out;
  for (usize i = 0; i != TArrayLen1 - 1 /* we skip the nullterminator here*/; ++i)
  {
    out[i] = a[i];
  }
  for (usize i = 0; i != TArrayLen2; ++i)
  {
    out[i + TArrayLen1 - 1] = b[i];
  }
  return out;
}

/*
@FIXME: might be borken
*/
template <std::size_t N>
constexpr std::array<char, N>
string_view_to_array(std::string_view str)
{
  std::array<char, N> arr{};
  std::size_t i = 0;

  for (; i < str.size() && i < N; ++i)
  {
    arr[i] = str[i];
  }

  // The remaining elements are already zero-initialized to '\0'
  return arr;
}

inline std::string
nullterminate_view(const std::string_view& view)
{
  return std::string(view);
}

template <typename TCharArr, std::size_t TSize>
struct ct_string
{
  consteval ct_string(const TCharArr (&arr)[TSize]) : data{std::to_array(arr)} {}
  consteval ct_string(const std::array<TCharArr, TSize> arr) : data{arr} {}
  consteval ct_string(const ct_string<TCharArr, TSize>& other) : data{other.data} {}

  consteval ct_string(const std::string_view& sv) : data{string_view_to_array<TSize>(sv)} {}

  template <usize TOtherSize, usize TOtherSize1>
  consteval ct_string(const ct_string<TCharArr, TOtherSize>& other0, const ct_string<TCharArr, TOtherSize1>& other1)
      : data{concat_arrays(other0.data, other1.data)}
  {
  }

  [[nodiscard]] constexpr std::string_view
  to_view() const noexcept
  {
    return std::string_view{data.begin(), data.end() - 1};
  }

  const std::string
  to_string() const
  {
    return std::string(data.data());
  }

  std::string
  to_string()
  {
    return std::string(data.data());
  }

  static constexpr usize size = TSize;
  const std::array<TCharArr, TSize> data;
};
