#pragma once

#include <cassert>
#include <include/wren/wren.hpp>

#include <include/FastNoise.h>

#include <common.h>
#include <unordered_map>
#include <stack>

namespace scripting
{

namespace noise_impl
{

static inline std::unordered_map<u32, FastNoiseLite> noise_handles;
static inline u32 last_handle = 1;
static inline std::stack<u32> returned_handles{};

static inline u32
get_new_handle()
{
  if (!returned_handles.empty())
  {
    auto top = returned_handles.top();
    returned_handles.pop();
    return top;
  }
  return last_handle++;
}

static inline u0
free_handle(u32 _handle)
{
  if (_handle == (last_handle + 1))
  {
    last_handle--;
  }
  else
  {
    returned_handles.push(_handle);
  }
}
} // namespace noise_impl

static inline void
IMPL_getNewNoiseHandle(WrenVM* vm)
{
  u32 handle = noise_impl::get_new_handle();
  noise_impl::noise_handles.insert({handle, {}});
  wrenEnsureSlots(vm, 1);
  wrenSetSlotDouble(vm, 0, (f64)handle);
}

static inline void
IMPL_freeNoiseHandle(WrenVM* vm)
{
  wrenEnsureSlots(vm, 2);
  auto handle = (u32)wrenGetSlotDouble(vm, 1);
  noise_impl::free_handle(handle);
  noise_impl::noise_handles.erase(handle);
}

// void setNoiseType(handle: int, type: str)
static inline void
IMPL_setNoiseType(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  auto handle = (u32)wrenGetSlotDouble(vm, 1);
  auto type   = std::string(wrenGetSlotString(vm, 2));

  if (type == "OpenSimplex2")
  {
    noise_impl::noise_handles.at(handle).SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  }
  else if (type == "OpenSimplex2S")
  {
    noise_impl::noise_handles.at(handle).SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
  }
  else if (type == "Cellular")
  {
    noise_impl::noise_handles.at(handle).SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  }
  else if (type == "Perlin")
  {
    noise_impl::noise_handles.at(handle).SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  }
  else if (type == "Value Cubic")
  {
    noise_impl::noise_handles.at(handle).SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
  }
  else if (type == "Value")
  {
    noise_impl::noise_handles.at(handle).SetNoiseType(FastNoiseLite::NoiseType_Value);
  }
  else
  {
    LOG(WARNING) << "[scripting] : (noiselib): trying to load incorrect noise type: " << type;
  }
}

// void setNoiseSeed(handle: int, seed: int)
static inline void
IMPL_setNoiseSeed(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto seed   = (i32)wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).SetSeed(seed);
}

// void setNoiseFreq(handle: int, freq: double)
static inline void
IMPL_setNoiseFreq(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto freq   = (f64)wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).SetFrequency(freq);
}

// double getNoise2d(handle: int, x: double, y: double)
static inline void
IMPL_getNoise2d(WrenVM* vm)
{
  wrenEnsureSlots(vm, 4);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto x      = (f64)wrenGetSlotDouble(vm, 2);
  const auto y      = (f64)wrenGetSlotDouble(vm, 3);
  f64 val           = noise_impl::noise_handles.at(handle).GetNoise(x, y);
  wrenSetSlotDouble(vm, 0, val);
}

// double getNoise2d(handle: int, x: double, y: double)
static inline void
IMPL_getNoise3d(WrenVM* vm)
{
  wrenEnsureSlots(vm, 5);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto x      = (f64)wrenGetSlotDouble(vm, 2);
  const auto y      = (f64)wrenGetSlotDouble(vm, 3);
  const auto z      = (f64)wrenGetSlotDouble(vm, 4);
  f64 val           = noise_impl::noise_handles.at(handle).GetNoise(x, y, z);
  wrenSetSlotDouble(vm, 0, val);
}

static inline void
IMPL_Debugprint(WrenVM* vm)
{
  wrenEnsureSlots(vm, 2);
  auto str = std::string(wrenGetSlotString(vm, 1));
  std::cout << "[vm] >> " << str << "\n";
}

static inline WrenForeignMethodFn
bindForeignMethod(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature)
{
  /*
    noiselib functions
  */
  if (strcmp(module, "dbglib") == 0)
  {
    if (strcmp(className, "Debug") == 0)
    {
      if (isStatic && strcmp(signature, "print(_)") == 0)
      {
        return IMPL_Debugprint;
      }
    }
  }
  else if (strcmp(module, "noiselib") == 0)
  {
    /*
      Noise class:

      int getNewNoiseHandle()
      void freeNoiseHandle(handle: int)

      NoiseImpl class:

      void setNoiseType(handle: int, type: str)
      void setNoiseSeed(handle: int, seed: int)
      void setNoiseFreq(handle: int, freq: double)

      double getNoise2d(handle: int, x: double, y: double)
      double getNoise3d(handle: int, x: double, y: double, z: double)

    */
    if (strcmp(className, "Noise") == 0)
    {
      if (strcmp(signature, "getNewNoiseHandle()") == 0)
      {
        return IMPL_getNewNoiseHandle;
      }
      else if (strcmp(signature, "freeNoiseHandle(_)") == 0)
      {
        return IMPL_freeNoiseHandle;
      }
    }
    else if (strcmp(className, "NoiseImpl") == 0)
    {
      if (isStatic && strcmp(signature, "setNoiseType(_,_)") == 0)
      {
        return IMPL_setNoiseType;
      }
      else if (isStatic && strcmp(signature, "setNoiseSeed(_,_)") == 0)
      {
        return IMPL_setNoiseSeed;
      }
      else if (isStatic && strcmp(signature, "setNoiseFreq(_,_)") == 0)
      {
        return IMPL_setNoiseFreq;
      }
      else if (isStatic && strcmp(signature, "getNoise2d(_,_,_)") == 0)
      {
        return IMPL_getNoise2d;
      }
      else if (isStatic && strcmp(signature, "getNoise3d(_,_,_,_)") == 0)
      {
        return IMPL_getNoise3d;
      }
      else
      {
        puts("wtf");
        assert(false);
      }
      // Other foreign methods on Math...
    }
    // Other classes in main...
  }
  // Other modules...
  return nullptr;
}

} // namespace scripting
