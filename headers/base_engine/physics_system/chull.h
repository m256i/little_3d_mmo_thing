#pragma once

#include <cmath>
#include <exception>
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtx/intersect.hpp>

#include <base_engine/renderer/mesh.h>
#include <base_engine/physics_system/bbox.h>
#include <base_engine/physics_system/mesh_collider.h>
#include <base_engine/debug/debug_overlay.h>

#include "../../common.h"
#include "glm/geometric.hpp"

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

  u0
  setup(usize _count_rows, usize _count_columns, usize _count_stories)
  {
    count_rows    = _count_rows;
    count_columns = _count_columns;
    count_stories = _count_stories;
    // mental health: 90%
    grid.resize(_count_stories);
    for (auto& column : grid)
    {
      column.resize(_count_columns);
      for (auto& row : column)
      {
        row.resize(_count_rows);
      }
    }
    // mental health: 85%
  }

  u0
  generate(const aabb_t& _bbox, const std::vector<triangle_t>& _triangles, usize _tri_count)
  {
    bbox = std::move(_bbox);
    std::deque<triangle_t> tris;

    for (const auto& tri : _triangles)
    {
      tris.push_back(tri);
    }

    // rows
    f32 box_x_len = bbox.max.x - bbox.min.x;
    // stories
    f32 box_y_len = bbox.max.y - bbox.min.y;
    // columns
    f32 box_z_len = bbox.max.z - bbox.min.z;

    const f32 voxel_story_length  = box_y_len / count_stories;
    const f32 voxel_column_length = box_z_len / count_columns;
    const f32 voxel_row_length    = box_x_len / count_rows;

    // TODO: use triangles instead of points

    // mental health: 87% got some coffee :D
    usize story_index{0};
    for (const auto& stories : grid)
    {
      usize column_index{0};
      for (const auto& column : stories)
      {
        usize row_index{0};
        for (const auto& row : column)
        {
          // mental health: 74%
          auto voxel_min = bbox.min + glm::vec3{voxel_row_length * (f32)row_index, voxel_story_length * (f32)story_index,
                                                voxel_column_length * (f32)column_index};

          auto voxel_max = bbox.min + glm::vec3{voxel_row_length * (f32)(row_index + 1), voxel_story_length * (f32)(story_index + 1),
                                                voxel_column_length * (f32)(column_index + 1)};

          aabb_t current_voxel{voxel_min, voxel_max};

          for (auto& tri : tris)
          {
            // mental health: 71%

            // TODO: fix: broken burh
            static constexpr auto tri_inside_bbox = [](const aabb_t& _box, const triangle_t& _tri, f32 _thresold)
            {
              static constexpr auto point_inside_bbox = [](const aabb_t& _box, const glm::vec3& _point) -> bool
              {
                // bruh
                return (_point.x >= _box.min.x && _point.y >= _box.min.y && _point.z >= _box.min.z) &&
                       (_point.x <= _box.max.x && _point.y <= _box.max.y && _point.z <= _box.max.z);
              };

              static constexpr auto calculate_tri_area_ = [](const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
              {
                glm::vec3 v0 = p1 - p0;
                glm::vec3 v1 = p2 - p0;
                return 0.5f * glm::length(glm::cross(v0, v1));
              };

              static constexpr auto calculate_tri_area = [](const triangle_t& triangle)
              { return calculate_tri_area_(triangle.a, triangle.b, triangle.c); };

              static auto calcuate_intersection_area = [&](const triangle_t& triangle, const aabb_t& bbox)
              {
                glm::vec3 baryPosition;
                bool intersection = glm::intersectLineTriangle(bbox.min, glm::normalize(triangle.a - bbox.min), triangle.a, triangle.b,
                                                               triangle.c, baryPosition);

                if (!intersection)
                {
                  return 0.0f;
                }

                glm::vec3 intersectionPoint =
                    triangle.a + baryPosition.x * (triangle.b - triangle.a) + baryPosition.y * (triangle.c - triangle.a);
                f32 intersectionArea = calculate_tri_area_(intersectionPoint, triangle.b, triangle.c);
                return intersectionArea;
              };

              static constexpr auto calculate_percentage = [](const triangle_t& triangle, const aabb_t& bbox)
              {
                f32 intersectionArea = calcuate_intersection_area(triangle, bbox);
                f32 triangleArea     = calculate_tri_area(triangle);
                f32 percentage       = (intersectionArea / triangleArea);
                return percentage;
              };

              return (calculate_percentage(_tri, _box) >= _thresold);
            };

            /* % of the triangle that has to be inside of the box to count */
            if (tri_inside_bbox(current_voxel, tri, 20.f))
            {
              grid[story_index][column_index][row_index] = true;
              // mental health: 32%
            }
          }
          ++row_index;
        }
        ++column_index;
      }
      ++story_index;
    }
  }

  void
  draw()
  {
    // rows
    f32 box_x_len = bbox.max.x - bbox.min.x;
    // stories
    f32 box_y_len = bbox.max.y - bbox.min.y;
    // columns
    f32 box_z_len = bbox.max.z - bbox.min.z;

    const f32 voxel_story_length  = box_y_len / count_stories;
    const f32 voxel_column_length = box_z_len / count_columns;
    const f32 voxel_row_length    = box_x_len / count_rows;

    usize story_index{0};
    for (const auto& stories : grid)
    {
      usize column_index{0};
      for (const auto& column : stories)
      {
        usize row_index{0};
        for (const auto& row : column)
        {

          auto voxel_min = bbox.min + glm::vec3{voxel_row_length * (f32)row_index, voxel_story_length * (f32)story_index,
                                                voxel_column_length * (f32)column_index};

          auto voxel_max = bbox.min + glm::vec3{voxel_row_length * (f32)(row_index + 1), voxel_story_length * (f32)(story_index + 1),
                                                voxel_column_length * (f32)(column_index + 1)};

          aabb_t current_voxel{voxel_min, voxel_max};

          if (!row)
          {
            debug_overlay_t::draw_AABB(current_voxel.min, current_voxel.max, 0xffffffff, true);
          }

          ++row_index;
        }
        ++column_index;
      }
      ++story_index;
    }
  }
};
