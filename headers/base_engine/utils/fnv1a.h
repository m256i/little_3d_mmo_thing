#pragma once

#include <common.h>

namespace fnv1a
{
class hash
{
  static const u32 FNV_PRIME    = 16777619u;
  static const u32 OFFSET_BASIS = 2166136261u;
  template <u32 N>
  static constexpr u32
  fnvHashConst(const char (&str)[N], u32 I = N)
  {
    return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnvHashConst(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
  }
  static u32
  fnvHash(const char* str)
  {
    const size_t length = strlen(str) + 1;
    u32 hash            = OFFSET_BASIS;
    for (size_t i = 0; i < length; ++i)
    {
      hash ^= *str++;
      hash *= FNV_PRIME;
    }
    return hash;
  }
  struct Wrapper
  {
    Wrapper(const char* str) : str(str) {}
    const char* str;
  };
  u32 hash_value;

public:
  // calulate in run-time
  hash(Wrapper wrapper) : hash_value(fnvHash(wrapper.str)) {}
  // calulate in compile-time
  template <u32 N>
  constexpr hash(const char (&str)[N]) : hash_value(fnvHashConst(str))
  {
  }
  // output result
  constexpr
  operator u32() const
  {
    return this->hash_value;
  }
};
} // namespace fnv1a
