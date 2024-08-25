#pragma once

#include <common.h>
#include <vector>
#include <tuple>
#include <type_traits>

namespace utils
{

constexpr decltype(auto)
zip(auto& container)
{
  using ret_type =
      std::tuple<usize,
                 std::add_lvalue_reference_t<typename std::remove_all_extents_t<std::remove_cvref_t<decltype(container)>>::value_type>>;

  std::vector<ret_type> out;

  usize i = 0;
  for (auto& ite : container)
  {
    out.push_back(ret_type{i, (ite)});
    ++i;
  }

  return out;
}

constexpr decltype(auto)
zip(const auto& container)
{
  using ret_type = std::tuple<
      usize, const std::add_lvalue_reference_t<typename std::remove_all_extents_t<std::remove_cvref_t<decltype(container)>>::value_type>>;

  std::vector<ret_type> out;

  usize i = 0;
  for (const auto& ite : container)
  {
    out.push_back(ret_type{i, (ite)});
    ++i;
  }

  return out;
}

} // namespace utils