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

struct voxel_block_t
{
  aabb_t bbox{};
  std::vector<triangle_t> triangles_inside{};
  usize count{};
  bool visited{};
  bool on{};
};

struct voxel_grid_t
{
  aabb_t bbox;
  usize count_rows{}, count_columns{}, count_stories{};

  std::vector<std::vector<std::vector<voxel_block_t>>> grid;

  u0 setup(usize _count_rows, usize _count_columns, usize _count_stories);
  u0 generate(const aabb_t& _bbox, const std::vector<triangle_t>& _triangles, usize _tri_count);
  voxel_block_t* find(const glm::vec3& _position);
  u0 draw() const;
  std::pair<aabb_t, usize> fint_largest_sub_cuboid(std::vector<std::vector<std::vector<voxel_block_t>>>& _grid);
};
