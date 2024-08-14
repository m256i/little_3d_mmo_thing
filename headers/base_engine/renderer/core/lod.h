#pragma once

#include <common.h>

namespace lod
{
enum class detail_level : u32
{
  lod_detail_full,
  lod_detail_high,
  lod_detail_mid,
  lod_detail_low,
  lod_detail_potato,
  lod_detail_enum_size
};

static constexpr std::array detail_scales{1.f, 0.65f, 0.45f, 0.1f, 0.01f};
static constexpr std::array model_detail_scales{1.f, 0.70f, 0.4f, 0.3f, 0.2f};
static constexpr std::array model_world_scales{1.f, 0.5f, 0.5f, 0.5f, 0.5f};

} // namespace lod
