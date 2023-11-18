#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include <glm/vec3.hpp>
#include <deque>
#include <algorithm>
#include <numeric>
#include <limits>

#include <base_engine/physics_system/bbox.h>

#include "../../../common.h"

struct triangle_t
{
  union
  {
    struct
    {
      glm::vec3 a, b, c;
    };
    struct
    {
      glm::vec3 v0{}, v1{}, v2{};
    };
  };

  auto
  edge0() const
  {
    return (b - a);
  }
  auto
  edge1() const
  {
    return (c - b);
  }
  auto
  edge2() const
  {
    return (a - c);
  }

  auto
  aabb_bbox() const
  {
    const auto& _tri = *this;
    f32 lowest_x = std::numeric_limits<f32>::max(), lowest_z = std::numeric_limits<f32>::max(), lowest_y = std::numeric_limits<f32>::max();

    f32 highest_x = std::numeric_limits<f32>::min(), highest_y = std::numeric_limits<f32>::min(),
        highest_z = std::numeric_limits<f32>::min();

    highest_x = std::max({_tri.a.x, _tri.b.x, _tri.c.x});
    highest_y = std::max({_tri.a.y, _tri.b.y, _tri.c.y});
    highest_z = std::max({_tri.a.z, _tri.b.z, _tri.c.z});

    lowest_x = std::min({_tri.a.x, _tri.b.x, _tri.c.x});
    lowest_y = std::min({_tri.a.y, _tri.b.y, _tri.c.y});
    lowest_z = std::min({_tri.a.z, _tri.b.z, _tri.c.z});

    return aabb_t{glm::vec3{lowest_x, lowest_y, lowest_z}, glm::vec3{highest_x, highest_y, highest_z}};
  }
};