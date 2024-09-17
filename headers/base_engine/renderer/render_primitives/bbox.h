#pragma once

#include <common.h>
#include <glm/glm.hpp>
#include <base_engine/debug/debug_overlay.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace primitives
{
struct aabb
{
  f32
  get_max_dimension()
  {
    auto size = max - min;
    return glm::max(size.x, glm::max(size.y, size.z));
  }

  glm::vec3
  get_extents() const
  {
    return (max - min) / 2.f;
  }

  u0
  expand_to_bbox(const auto& _min, const decltype(_min)& _max)
    requires requires(decltype(_min) t) {
      t.x;
      t.y;
      t.z;
    }
  {
    const auto nmin = glm::vec3{_min.x, _min.y, _min.z};
    const auto nmax = glm::vec3{_max.x, _max.y, _max.z};

    if (nmin.x < min.x) min.x = nmin.x;
    if (nmin.y < min.y) min.y = nmin.y;
    if (nmin.z < min.z) min.z = nmin.z;

    if (nmax.x > max.x) max.x = nmax.x;
    if (nmax.y > max.y) max.y = nmax.y;
    if (nmax.z > max.z) max.z = nmax.z;
  }

  auto
  get_center() -> glm::vec3
  {
    return (min + max) / 2.f;
  }

  glm::vec3 min, max;
};

struct obb
{
  glm::vec3 center;
  std::array<glm::vec3, 3> axes;
  glm::vec3 mins, maxs;

  u0
  debug_draw_axes()
  {
    glPointSize(5.f);
    debug_overlay_t::draw_point(center, 0xff0000ff, true);
    glPointSize(1.f);
    glLineWidth(3.f);
    debug_overlay_t::draw_line({center + axes[0] * mins.x, center + axes[0] * maxs.x}, 0xff0f0fff, true);
    debug_overlay_t::draw_line({center + axes[1] * mins.y, center + axes[1] * maxs.y}, 0x0fff0fff, true);
    debug_overlay_t::draw_line({center + axes[2] * mins.z, center + axes[2] * maxs.z}, 0x0f0fffff, true);
    glLineWidth(1.f);
  }
};

} // namespace primitives