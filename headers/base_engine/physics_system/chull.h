#pragma once

#include <algorithm>
#include <cmath>
#include <exception>
#include <glm/glm.hpp>
#include <limits>
#include <vector>
#include <glm/gtx/intersect.hpp>

#include <base_engine/renderer/mesh.h>
#include <base_engine/physics_system/bbox.h>
#include <base_engine/physics_system/base_shapes.h>
#include <base_engine/physics_system/mesh_collider.h>
#include <base_engine/debug/debug_overlay.h>

#include "../../common.h"

struct convex_hull_t
{
  convex_hull_t() = default;
  std::vector<glm::vec3> points{};
  std::vector<usize> indices{};
  std::vector<std::vector<glm::vec3>> simplified_chulls{};
  glm::vec3 center{};

  u0 load(const mesh_t& _mesh);
  u0 from_mesh(const mesh_t& _mesh);
  std::vector<glm::vec3> reduce(const std::vector<glm::vec3>& _vertices, double _min_distance = 0.f);
  bool to_submeshes();
  bool simplify();
};

struct voxel_grid_t
{
  aabb_t bbox;
  usize count_rows{}, count_columns{}, count_stories{};
  std::vector<std::vector<std::vector<bool>>> grid;

  u0 setup(usize _count_rows, usize _count_columns, usize _count_stories);
  u0 generate(const aabb_t& _bbox, const std::vector<triangle_t>& _triangles, usize _tri_count);
  u0 draw() const;
};
