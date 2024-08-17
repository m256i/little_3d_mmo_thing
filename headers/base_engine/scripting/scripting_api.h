#pragma once

#include <cassert>
#include <include/wren/wren.hpp>

#include <include/FastNoise.h>

#include <common.h>
#include <unordered_map>
#include <stack>

#include "../utils/fnv1a.h"

// @TODO: @FIXME: HUGE FIXME here please make this actualy good and not spagetti code and stupid

namespace scripting
{
namespace noise_impl
{

struct noise_instance
{
  FastNoiseLite noise, warp;
  bool apply_warp = false;
};

static inline std::unordered_map<u32, noise_instance> noise_handles;
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
  using namespace fnv1a;

  wrenEnsureSlots(vm, 3);
  auto handle = (u32)wrenGetSlotDouble(vm, 1);
  auto type   = hash(wrenGetSlotString(vm, 2));

  switch (type)
  {
  case hash("OpenSimplex2"):
  {
    noise_impl::noise_handles.at(handle).noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    break;
  }
  case hash("OpenSimplex2S"):
  {
    noise_impl::noise_handles.at(handle).noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    break;
  }
  case hash("Cellular"):
  {
    noise_impl::noise_handles.at(handle).noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    break;
  }
  case hash("Perlin"):
  {
    noise_impl::noise_handles.at(handle).noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    break;
  }
  case hash("Value Cubic"):
  {
    noise_impl::noise_handles.at(handle).noise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
    break;
  }
  case hash("Value"):
  {
    noise_impl::noise_handles.at(handle).noise.SetNoiseType(FastNoiseLite::NoiseType_Value);
    break;
  }
  default:
  {
    LOG(WARNING) << "[scripting] : (noiselib): trying to load incorrect noise type: " << type;
    break;
  }
  }
}

// void setNoiseSeed(handle: int, seed: int)
static inline void
IMPL_setNoiseSeed(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto seed   = (i32)wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).noise.SetSeed(seed);
}

// void setNoiseFreq(handle: int, freq: double)
static inline void
IMPL_setNoiseFreq(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto freq   = (f64)wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).noise.SetFrequency(freq);
}

// void setFractalType(handle: int, str type)
static inline void
IMPL_setFractalType(WrenVM* vm)
{
  using namespace fnv1a;

  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto type   = hash(wrenGetSlotString(vm, 2));

  switch (type)
  {
  case hash("None"):
    noise_impl::noise_handles.at(handle).noise.SetFractalType(FastNoiseLite::FractalType_None);
    break;
  case hash("FBm"):
    noise_impl::noise_handles.at(handle).noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    break;
  case hash("Ridged"):
    noise_impl::noise_handles.at(handle).noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    break;
  case hash("Ping Pong"):
    noise_impl::noise_handles.at(handle).noise.SetFractalType(FastNoiseLite::FractalType_PingPong);
    break;
  default:
  {
    LOG(DEBUG) << "[scripting] : (noiselib) : unknown fractal type: " << wrenGetSlotString(vm, 2);
  }
  }
}

// void setFractalOctaves(handle: int, int: octaves)
static inline void
IMPL_setFractalOctaves(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle  = (u32)wrenGetSlotDouble(vm, 1);
  const auto octaves = (u32)wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).noise.SetFractalOctaves(octaves);
}

// void setFractalLacunarity(handle: int, double: octaves)
static inline void
IMPL_setFractalLacunarity(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto lac    = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).noise.SetFractalLacunarity(lac);
}

// void setFractalGain(handle: int, double: gain)
static inline void
IMPL_setFractalGain(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto gain   = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).noise.SetFractalGain(gain);
}

// void setFractalWStrength(handle: int, double: strength)
static inline void
IMPL_setFractalWStrength(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle   = (u32)wrenGetSlotDouble(vm, 1);
  const auto strength = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).noise.SetFractalWeightedStrength(strength);
}

// void setFractalPPStrength(handle: int, double: ppstrength)
static inline void
IMPL_setFractalPPStrength(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle     = (u32)wrenGetSlotDouble(vm, 1);
  const auto ppstrength = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).noise.SetFractalPingPongStrength(ppstrength);
}

// void setCellularDistFunc(handle: int, str: dist_func)
static inline void
IMPL_setCellularDistFunc(WrenVM* vm)
{
  using namespace fnv1a;

  wrenEnsureSlots(vm, 3);
  const auto handle    = (u32)wrenGetSlotDouble(vm, 1);
  const auto dist_func = hash(wrenGetSlotString(vm, 2));
  switch (dist_func)
  {
  case hash("Euclidian"):
    noise_impl::noise_handles.at(handle).noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
    break;
  case hash("Euclidian Sq"):
    noise_impl::noise_handles.at(handle).noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
    break;
  case hash("Manhattan"):
    noise_impl::noise_handles.at(handle).noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
    break;
  case hash("Hybrid"):
    noise_impl::noise_handles.at(handle).noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid);
    break;
  default:
  {
    LOG(DEBUG) << "[scripting] : (noiselib) : unknown cell. dist. function: " << wrenGetSlotString(vm, 2);
  }
  }
}

// void setCellularRetType(handle: int, str: type)
static inline void
IMPL_setCellularRetType(WrenVM* vm)
{
  using namespace fnv1a;

  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto type   = hash(wrenGetSlotString(vm, 2));
  switch (type)
  {
  case hash("Cell Value"):
    noise_impl::noise_handles.at(handle).noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
    break;
  case hash("Distance"):
    noise_impl::noise_handles.at(handle).noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
    break;
  case hash("Distance 2"):
    noise_impl::noise_handles.at(handle).noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2);
    break;
  case hash("Distance 2 Add"):
    noise_impl::noise_handles.at(handle).noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Add);
    break;
  case hash("Distance 2 Sub"):
    noise_impl::noise_handles.at(handle).noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Sub);
    break;
  case hash("Distance 2 Mul"):
    noise_impl::noise_handles.at(handle).noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Mul);
    break;
  case hash("Distance 2 Div"):
    noise_impl::noise_handles.at(handle).noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Div);
    break;
  default:
  {
    LOG(DEBUG) << "[scripting] : (noiselib) : unknown cell. return type: " << wrenGetSlotString(vm, 2);
  }
  }
}

// void setCellularJitter(handle: int, double: jitter)
static inline void
IMPL_setCellularJitter(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto jitter = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).noise.SetCellularJitter(jitter);
}

// void setDomainWarpType(handle: int, str: type)
static inline void
IMPL_setDomainWarpType(WrenVM* vm)
{
  using namespace fnv1a;

  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto type   = hash(wrenGetSlotString(vm, 2));

  noise_impl::noise_handles.at(handle).apply_warp = true;

  switch (type)
  {
  case hash("None"):
    noise_impl::noise_handles.at(handle).apply_warp = false;
    break;
  case hash("OpenSimplex2"):
    noise_impl::noise_handles.at(handle).warp.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
    break;
  case hash("OpenSimplex2 Reduced"):
    noise_impl::noise_handles.at(handle).warp.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2Reduced);
    break;
  case hash("Basic Grid"):
    noise_impl::noise_handles.at(handle).warp.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2Reduced);
    break;
  default:
  {
    LOG(DEBUG) << "[scripting] : (noiselib) : unknown domain warp type: " << wrenGetSlotString(vm, 2);
  }
  }
}

// void setDomainWarpRotationType3d(handle: int, str: type)
static inline void
IMPL_setDomainWarpRotationType3d(WrenVM* vm)
{
  using namespace fnv1a;

  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto type   = hash(wrenGetSlotString(vm, 2));

  switch (type)
  {
  case hash("None"):
    noise_impl::noise_handles.at(handle).warp.SetRotationType3D(FastNoiseLite::RotationType3D_None);
    break;
  case hash("Improve XY Planes"):
    noise_impl::noise_handles.at(handle).warp.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
    break;
  case hash("Improve XZ Planes"):
    noise_impl::noise_handles.at(handle).warp.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXZPlanes);
    break;
  default:
  {
    LOG(DEBUG) << "[scripting] : (noiselib) : unknown domain warp rotation type: " << wrenGetSlotString(vm, 2);
  }
  }
}

// void setDomainWarpAmplitude(handle: int, double: amp)
static inline void
IMPL_setDomainWarpAmplitude(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto amp    = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).warp.SetDomainWarpAmp(amp);
}

// void setDomainWarpSeed(handle: int, int: seed)
static inline void
IMPL_setDomainWarpSeed(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto seed   = (u32)wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).warp.SetSeed(seed);
}

// void setDomainWarpFrequency(handle: int, double: freq)
static inline void
IMPL_setDomainWarpFrequency(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto freq   = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).warp.SetFrequency(freq);
}

// void setDomainWarpFractalType(handle: int, str: type)
static inline void
IMPL_setDomainWarpFractalType(WrenVM* vm)
{
  using namespace fnv1a;

  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto type   = hash(wrenGetSlotString(vm, 2));

  switch (type)
  {
  case hash("None"):
    noise_impl::noise_handles.at(handle).warp.SetFractalType(FastNoiseLite::FractalType_None);
    break;
  case hash("Progressive"):
    noise_impl::noise_handles.at(handle).warp.SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive);
    break;
  case hash("Independent"):
    noise_impl::noise_handles.at(handle).warp.SetFractalType(FastNoiseLite::FractalType_DomainWarpIndependent);
    break;
  default:
  {
    LOG(DEBUG) << "[scripting] : (noiselib) : unknown domain warp fractal type: " << wrenGetSlotString(vm, 2);
  }
  }
}

// void setDomainWarpFractalOctaves(handle: int, int: octaves)
static inline void
IMPL_setDomainWarpFractalOctaves(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle  = (u32)wrenGetSlotDouble(vm, 1);
  const auto octaves = (u32)wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).warp.SetFractalOctaves(octaves);
}

// void setDomainWarpFractalLacunarity(handle: int, double: lac)
static inline void
IMPL_setDomainWarpFractalLacunarity(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto lac    = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).warp.SetFractalLacunarity(lac);
}

// void setDomainWarpFractalGain(handle: int, double: gain)
static inline void
IMPL_setDomainWarpFractalGain(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  const auto gain   = wrenGetSlotDouble(vm, 2);
  noise_impl::noise_handles.at(handle).warp.SetFractalGain(gain);
}

// double getNoise2d(handle: int, x: double, y: double)
static inline void
IMPL_getNoise2d(WrenVM* vm)
{
  wrenEnsureSlots(vm, 4);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  auto x            = (f64)wrenGetSlotDouble(vm, 2);
  auto y            = (f64)wrenGetSlotDouble(vm, 3);

  if (noise_impl::noise_handles.at(handle).apply_warp)
  {
    noise_impl::noise_handles.at(handle).warp.DomainWarp(x, y);
  }

  f64 val = noise_impl::noise_handles.at(handle).noise.GetNoise(x, y);
  wrenSetSlotDouble(vm, 0, val);
}

// double getNoise2d(handle: int, x: double, y: double)
static inline void
IMPL_getNoise3d(WrenVM* vm)
{
  wrenEnsureSlots(vm, 5);
  const auto handle = (u32)wrenGetSlotDouble(vm, 1);
  auto x            = (f64)wrenGetSlotDouble(vm, 2);
  auto y            = (f64)wrenGetSlotDouble(vm, 3);
  auto z            = (f64)wrenGetSlotDouble(vm, 4);

  if (noise_impl::noise_handles.at(handle).apply_warp)
  {
    noise_impl::noise_handles.at(handle).warp.DomainWarp(x, y, z);
  }

  f64 val = noise_impl::noise_handles.at(handle).noise.GetNoise(x, y, z);
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
  using namespace fnv1a;

  const auto module_hash = hash(module);
  const auto class_hash  = hash(className);
  const auto sig_hash    = hash(signature);

  switch (module_hash)
  {
  case hash("dbglib"):
  {
    switch (class_hash)
    {
    case hash("Debug"):
    {
      switch (sig_hash)
      {
      case hash("print(_)"):
      {
        return IMPL_Debugprint;
      }
      default:
      {
        assert(false);
      }
      }
      break;
    }
    }
    break;
  }
  case hash("noiselib"):
  {
    switch (class_hash)
    {
    case hash("Noise"):
    {
      switch (sig_hash)
      {
      case hash("getNewNoiseHandle()"):
      {
        return IMPL_getNewNoiseHandle;
      }
      case hash("freeNoiseHandle(_)"):
      {
        return IMPL_freeNoiseHandle;
      }
      default:
      {
        printf("Noise unknown function called: %s", signature);
        assert(false);
      }
      }
      break;
    }
    case hash("NoiseImpl"):
    {
      switch (sig_hash)
      {
      case hash("setNoiseType(_,_)"):
        return IMPL_setNoiseType;
      case hash("setNoiseSeed(_,_)"):
        return IMPL_setNoiseSeed;
      case hash("setNoiseFreq(_,_)"):
        return IMPL_setNoiseFreq;
      case hash("getNoise2d(_,_,_)"):
        return IMPL_getNoise2d;
      case hash("getNoise3d(_,_,_,_)"):
        return IMPL_getNoise3d;
      case hash("setFractalType(_,_)"):
        return IMPL_setFractalType;
      case hash("setFractalOctaves(_,_)"):
        return IMPL_setFractalOctaves;
      case hash("setFractalLacunarity(_,_)"):
        return IMPL_setFractalLacunarity;
      case hash("setFractalGain(_,_)"):
        return IMPL_setFractalGain;
      case hash("setFractalWStrength(_,_)"):
        return IMPL_setFractalWStrength;
      case hash("setFractalPPStrength(_,_)"):
        return IMPL_setFractalPPStrength;
      case hash("setCellularDistFunc(_,_)"):
        return IMPL_setCellularDistFunc;
      case hash("setCellularRetType(_,_)"):
        return IMPL_setCellularRetType;
      case hash("setCellularJitter(_,_)"):
        return IMPL_setCellularJitter;
      case hash("setDomainWarpType(_,_)"):
        return IMPL_setDomainWarpType;
      case hash("setDomainWarpRotationType3d(_,_)"):
        return IMPL_setDomainWarpRotationType3d;
      case hash("setDomainWarpAmplitude(_,_)"):
        return IMPL_setDomainWarpAmplitude;
      case hash("setDomainWarpSeed(_,_)"):
        return IMPL_setDomainWarpSeed;
      case hash("setDomainWarpFrequency(_,_)"):
        return IMPL_setDomainWarpFrequency;
      case hash("setDomainWarpFractalType(_,_)"):
        return IMPL_setDomainWarpFractalType;
      case hash("setDomainWarpFractalOctaves(_,_)"):
        return IMPL_setDomainWarpFractalOctaves;
      case hash("setDomainWarpFractalLacunarity(_,_)"):
        return IMPL_setDomainWarpFractalLacunarity;
      case hash("setDomainWarpFractalGain(_,_)"):
        return IMPL_setDomainWarpFractalGain;
      default:
      {
        printf("NoiseImpl unknown function called: %s", signature);
        assert(false);
        // __builtin_unreachable();
        break;
      }
      }
      break;
    }
    default:
    {
      break;
    }
    }
    break;
  }
  default:
    break;
  }
  assert(false);
  return nullptr;
}
} // namespace scripting
