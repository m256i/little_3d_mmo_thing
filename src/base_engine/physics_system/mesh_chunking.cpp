#include <base_engine/physics_system/mesh_chunking.h>
#include <base_engine/physics_system/intersect.h>
#include <cmath>

u0
voxel_grid_t::setup(usize _count_rows, usize _count_columns, usize _count_stories)
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
voxel_grid_t::generate(const aabb_t& _bbox, const std::vector<triangle_t>& _triangles, usize _tri_count)
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
  for (auto& stories : grid)
  {
    usize column_index{0};
    for (auto& column : stories)
    {
      usize row_index{0};
      for (auto& row : column)
      {
        // mental health: 74%
        auto voxel_min = bbox.min + glm::vec3{voxel_row_length * (f32)row_index, voxel_story_length * (f32)story_index,
                                              voxel_column_length * (f32)column_index};

        auto voxel_max = bbox.min + glm::vec3{voxel_row_length * (f32)(row_index + 1), voxel_story_length * (f32)(story_index + 1),
                                              voxel_column_length * (f32)(column_index + 1)};

        aabb_t current_voxel{voxel_min, voxel_max};
        row.bbox = current_voxel;

        for (auto& tri : tris)
        {
          /* % of the triangle that has to be inside of the box to count */
          if (isect_tri_aabb(tri, current_voxel))
          {
            row.triangles_inside.push_back(tri);
            row.on = true;
            row.count++;
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

u0
voxel_grid_t::draw() const
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

        if (row.on)
        {
          debug_overlay_t::draw_AABB(current_voxel.min, current_voxel.max, 0xf4f4f4ff, false);
        }

        ++row_index;
      }
      ++column_index;
    }
    ++story_index;
  }
}

voxel_block_t*
voxel_grid_t::find(const glm::vec3& _position)
{
  if (!point_inside_bbox(bbox, _position))
  {
    return nullptr;
  }

  f32 x_length = std::fabsf(bbox.max.x - bbox.min.x);
  f32 y_length = std::fabsf(bbox.max.y - bbox.min.y);
  f32 z_length = std::fabsf(bbox.max.z - bbox.min.z);

  f32 row_distance    = std::fabsf(std::fabsf(_position.x) - std::fabsf(bbox.min.x));
  f32 story_distance  = std::fabsf(std::fabsf(_position.y) - std::fabsf(bbox.min.y));
  f32 column_distance = std::fabsf(std::fabsf(_position.z) - std::fabsf(bbox.min.z));

  usize row_index    = (usize)(row_distance / (x_length / count_rows));
  usize story_index  = (usize)(story_distance / (y_length / count_stories));
  usize column_index = (usize)(column_distance / (z_length / count_columns));

  return &grid[story_index][column_index][row_index];
}