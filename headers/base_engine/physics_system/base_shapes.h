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
#include <glm/matrix.hpp>

#include "../../../common.h"
#include "glm/ext/quaternion_geometric.hpp"

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

  auto
  normal()
  {
    return glm::normalize(glm::cross(b - a, c - a));
  }

  float
  get_alignment() const
  {
    // Calculate the transformation matrix from the local space to world space.
    glm::mat4 transform = glm::mat4(1.0f);
    transform[0]        = glm::vec4(glm::normalize(edge0()), 0.0f);
    transform[1]        = glm::vec4(glm::normalize(edge1()), 0.0f);
    transform[2]        = glm::vec4(glm::normalize(edge2()), 0.0f);

    // Extract the rotation part of the matrix.
    glm::mat3 rotationMatrix = glm::mat3(transform);

    // Calculate the dot product of each world axis with the triangle normal.
    float alignmentX = glm::dot(rotationMatrix[0], glm::vec3(1.0f, 0.0f, 0.0f));
    float alignmentY = glm::dot(rotationMatrix[1], glm::vec3(0.0f, 1.0f, 0.0f));
    float alignmentZ = glm::dot(rotationMatrix[2], glm::vec3(0.0f, 0.0f, 1.0f));

    // Calculate the overall alignment as the average of the dot products.
    float overallAlignment = (alignmentX + alignmentY + alignmentZ) / 3.0f;

    return overallAlignment;
  }
};