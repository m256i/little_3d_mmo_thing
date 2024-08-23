#pragma once

#include <common.h>
#include <glm/glm.hpp>

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
} // namespace primitives