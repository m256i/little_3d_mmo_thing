#pragma once

#include <common.h>
#include <glm/glm.hpp>
#include <base_engine/debug/debug_overlay.h>
#include "core/math.h"
#include "static_world_model.h"
#include "static_render_model.h"

#include "core/ssbos.h"
#include <execution>

#include "render_primitives/bbox.h"
#include "camera.h"

struct instance_data
{
  primitives::obb bounding_box{};
  glm::vec3 world_position{};
  glm::vec3 world_scale{1.f, 1.f, 1.f};
  glm::vec3 world_rotation{};
  /* later maybe add more per instance data */
};

struct instanced_static_world_model
{
  static_world_model_t base_model{"instance_model"};
  usize instance_count{0};

  std::vector<instance_data> instance_data_buffer{};
  std::vector<glm::mat4> translated_instance_data{};

  instanced_static_world_model(usize _binding_point) : binding_point(_binding_point), gpu_buffer(_binding_point) {}

  usize binding_point;
  core::ssbo_t gpu_buffer;

  u0
  load_model(std::string_view _path)
  {
    base_model.load_model(_path);
  }

  u0
  init_shader(std::string_view vs_path, std::string_view fs_path)
  {
    base_model.init_shader(vs_path, fs_path);
  }

  u0
  set_instance_count(usize new_count)
  {
    if (new_count == instance_count)
    {
      return;
    }

    instance_count = new_count;
    instance_data_buffer.resize(new_count);
    translated_instance_data.resize(new_count);
    gpu_buffer.set_size(new_count * sizeof(glm::mat4));
  }

  u0
  buffer()
  {
    if (instance_count == 0)
    {
      return;
    }

    assert(instance_data_buffer.size() == instance_count);
    assert(translated_instance_data.size() == instance_count);

    gpu_buffer.buffer_data((u8 *)translated_instance_data.data(), sizeof(glm::mat4) * instance_count);
  }

  auto &
  get_instance_data()
  {
    return instance_data_buffer;
  }

  // TODO: possibly slow and needs some scaling optimizations as in threading or maybe a compute shader
  u0
  apply_translation_at(usize _index)
  {
    assert(_index < instance_data_buffer.size());

    auto &instance = instance_data_buffer.at(_index);

    glm::mat4 model = glm::mat4(1.0f);

    glm::vec3 rotation_axis = glm::cross(glm::vec3{0, 1, 0}, instance.world_rotation);
    float cos_angle         = glm::dot(glm::vec3{0, 1, 0}, instance.world_rotation);
    float angle             = glm::acos(cos_angle);

    model = glm::translate(model, instance.world_position);

    if (glm::length(rotation_axis) > 0.0001f)
    {
      model = glm::rotate(model, angle, rotation_axis);
      model = glm::rotate(model, (f32)rand(), {0, 1, 0});
    }

    /*
    TODO: implement bboc stuff
    */

    model = glm::scale(model, instance.world_scale);

    translated_instance_data.at(_index) = model;
  }

  u0
  reapply_all_translations()
  {
    for (usize i = 0; i != instance_data_buffer.size(); ++i)
    {
      apply_translation_at(i);
    }
  }

  u0
  draw(const glm::mat4 &_projection, const glm::mat4 &_view)
  {
    if (instance_count == 0)
    {
      return;
    }

    assert(instance_data_buffer.size() == instance_count);
    assert(translated_instance_data.size() == instance_count);

    gpu_buffer.bind();

    base_model.draw_no_model(_projection, _view, instance_count);
    gpu_buffer.unbind();
  }
};

namespace renderer
{

struct instanced_render_model
{
  std::vector<instance_data> instance_data_buffer{};
  std::vector<u16> instance_mappings{};
  std::vector<glm::mat4> translated_instance_data{};
  usize instance_count{1};
  ::core::ssbo_t gpu_buffer{1},
      /* the remap buffer acts as a layer of indirection to the model matrices so we can calculate the monce and then move around the data
         through 16 bit "pointers" */
      remap_buffer{2};

  /*
  TODO: only update the LOD stuff on mini chunk crossings
  */

  instanced_static_render_model base_model;

  u0
  load_model_from_file(std::string_view _path)
  {
    base_model.load_from_file(_path);
  }

  u0
  set_base_instance(usize _index)
  {
    base_model.pipeline.set_base_instance_index(_index);
  }

  u0
  set_instance_count(usize new_count)
  {
    if (new_count == instance_count)
    {
      return;
    }

    instance_count = new_count;
    instance_data_buffer.resize(new_count);
    instance_mappings.resize(new_count);
    translated_instance_data.resize(new_count);
    base_model.pipeline.set_instance_count(new_count);
    gpu_buffer.set_size(new_count * sizeof(glm::mat4));
    remap_buffer.set_size(new_count * sizeof(u16));
  }

  u0
  buffer()
  {
    if (instance_count == 0)
    {
      return;
    }

    assert(instance_data_buffer.size() == instance_count);
    assert(translated_instance_data.size() == instance_count);

    gpu_buffer.buffer_data((u8 *)translated_instance_data.data(), sizeof(glm::mat4) * instance_count);
    remap_buffer.buffer_data((u8 *)instance_mappings.data(), instance_mappings.size() * sizeof(u16));
  }

  auto &
  get_instance_data()
  {
    return instance_data_buffer;
  }

  // TODO: possibly slow and needs some scaling optimizations as in threading or maybe a compute shader
  u0
  apply_translation_at(usize _index)
  {
    assert(_index < instance_data_buffer.size());

    auto &instance = instance_data_buffer.at(_index);

    glm::mat4 model = glm::mat4(1.0f);

    glm::vec3 rotation_axis = glm::cross(glm::vec3{0, 1, 0}, instance.world_rotation);
    float cos_angle         = glm::dot(glm::vec3{0, 1, 0}, instance.world_rotation);
    float angle             = glm::acos(cos_angle);

    model = glm::translate(model, instance.world_position);

    if (glm::length(rotation_axis) > 0.0001f)
    {
      model = glm::rotate(model, angle, rotation_axis);
      model = glm::rotate(model, (f32)rand(), {0, 1, 0});
    }

    model = glm::scale(model, instance.world_scale);

    translated_instance_data.at(_index) = model;
    /*
    reset mappings
    */
    instance_mappings[_index] = (u16)_index;
  }

  u0
  remap(u16 _model_matrix_index, u16 _instance_id)
  {
    assert(_instance_id < instance_count);
    assert(_model_matrix_index < instance_count);
    instance_mappings[_instance_id] = _model_matrix_index;
  }

  u0
  reapply_all_translations()
  {
    for (usize i = 0; i != instance_data_buffer.size(); ++i)
    {
      apply_translation_at(i);
    }
  }

  u0
  draw(const auto &_camera)
  {
    if (instance_count == 0)
    {
      return;
    }

    assert(instance_data_buffer.size() == instance_count);
    assert(translated_instance_data.size() == instance_count);

    remap_buffer.bind();
    gpu_buffer.bind();
    base_model.draw(_camera);
    gpu_buffer.unbind();
    remap_buffer.unbind();
  }
};

struct instanced_render_model_lod
{
  /*
  we are using a grid to speed up frustum culling
  */
  std::vector<instance_data> instance_data_buffer{}; // basically holds OBB position scale and rotation
  std::vector<u16> instance_mappings{};
  std::vector<glm::mat4> translated_instance_data{};
  usize instance_count{1};
  ::core::ssbo_t gpu_buffer{1},
      /* the remap buffer acts as a layer of indirection to the model matrices so we can calculate the monce and then move around the data
         through 16 bit "pointers" */
      remap_buffer{2};

  /*
  TODO: only update the LOD stuff on mini chunk crossings
  */
  struct spacial_grid
  {
    // Initialize the spatial grid with given bounds, divisions, and instances
    void
    initialize_with_divs(const glm::vec3 &_min, const glm::vec3 &_max, usize _divs, const std::vector<instance_data> &_instances)
    {
      // 1. Set up grid bounds and divisions
      min       = _min;
      max       = _max;
      divisions = _divs;

      // 2. Set Y-axis divisions to be 1/3 of the X/Z divisions, but at least 1
      usize divisions_y = divisions / 3;

      // 3. Calculate the number of grid blocks and resize the blocks vector
      usize num_blocks = divisions * divisions_y * divisions;
      blocks.resize(num_blocks);

      // 4. Compute the size of each grid block
      glm::vec3 grid_size     = max - min; // Total size of the grid
      glm::vec3 block_size_xz = glm::vec3(grid_size.x / static_cast<f32>(divisions), 0.0f,
                                          grid_size.z / static_cast<f32>(divisions)); // Block size for X and Z axes
      f32 block_size_y        = grid_size.y / static_cast<f32>(divisions_y);          // Block size for Y axis

      // 5. Initialize each grid block's bounds (min and max)
      for (usize x = 0; x < divisions; ++x)
      {
        for (usize y = 0; y < divisions_y; ++y)
        {
          for (usize z = 0; z < divisions; ++z)
          {
            usize block_index = x + y * divisions + z * divisions * divisions_y;

            glm::vec3 block_min = min + glm::vec3(x, 0, z) * block_size_xz + glm::vec3(0.0f, y * block_size_y, 0.0f);
            glm::vec3 block_max = block_min + glm::vec3(block_size_xz.x, block_size_y, block_size_xz.z);

            blocks[block_index].min = block_min;
            blocks[block_index].max = block_max;
          }
        }
      }

      // 6. Assign instances to the appropriate grid blocks
      for (usize i = 0; i < _instances.size(); ++i)
      {
        const instance_data &instance = _instances[i];
        glm::vec3 instance_position   = instance.world_position; // Assuming instance_data has a position field

        // Compute grid indices for the instance based on its position
        glm::vec3 relative_pos = (instance_position - min) / grid_size; // Normalize instance position within grid space
        usize index_x          = static_cast<usize>(relative_pos.x * divisions);
        usize index_y          = static_cast<usize>(relative_pos.y * divisions_y);
        usize index_z          = static_cast<usize>(relative_pos.z * divisions);

        // Clamp indices to ensure they are within bounds
        index_x = glm::clamp(index_x, 0ull, divisions - 1);
        index_y = glm::clamp(index_y, 0ull, divisions_y - 1);
        index_z = glm::clamp(index_z, 0ull, divisions - 1);

        // Get the index of the grid block in the 1D vector
        usize block_index = index_x + index_y * divisions + index_z * divisions * divisions_y;

        // Add the instance index to the corresponding grid block
        blocks[block_index].contained_instance_indices.push_back(i);
      }
    }

    struct grid_block
    {
      HOT_PATH bool
      is_on_fwd_plane(primitives::aabb aabb, frust_plane plane) const
      {
        const auto extents = aabb.get_extents();
        const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);
        return -r <= plane.signed_dist_to_center(aabb.get_center());
      };

      HOT_PATH bool
      is_in_frustum(const camera_frust &frustum) const
      {
        const primitives::aabb box{min, max};

        return (is_on_fwd_plane(box, frustum.bottomFace) && is_on_fwd_plane(box, frustum.farFace) &&
                is_on_fwd_plane(box, frustum.leftFace) && is_on_fwd_plane(box, frustum.nearFace) &&
                is_on_fwd_plane(box, frustum.rightFace) && is_on_fwd_plane(box, frustum.topFace));
      }

      std::vector<u16> contained_instance_indices{};
      glm::vec3 min{}, max{};
      bool currently_active = false;
    };

    std::vector<grid_block> blocks{};

    u0
    debug_draw(const std::vector<instance_data> &_instances)
    {
      glLineWidth(5.f);
      /*
      adding ugly offsets because of z-fighting
      */
      debug_overlay_t::draw_AABB(min - glm::vec3{1, 1, 1}, max + glm::vec3{1, 1, 1}, 0xf30f0f05, true);
      usize i = 0;
      for (const auto &block : blocks)
      {
        if (!block.currently_active)
        {
          continue;
        }
        u32 col = (i * 123445357) % 0xFFFFFF;      // Use a unique block index to generate a unique color
        col     = (col & 0xFFFFFF00) | 0x000000FF; // Set the alpha channel to fully opaque
        debug_overlay_t::draw_AABB(block.min, block.max, col, true);
        for (const auto &point : block.contained_instance_indices)
        {
          glPointSize(5.f);
          debug_overlay_t::draw_point(_instances[point].world_position, col, true);
          glPointSize(1.f);
        }
        i++;
      }
      glLineWidth(1.f);
    }

    usize divisions{1}; /* 1 here means that there are no divisions since x/1=x */
    glm::vec3 min{}, max{};
  };

  struct lod_instance_data
  {
    instanced_static_render_model_lod model{};
    usize instance_count{}, actual_instance_count{};
    usize instace_base_index{};
  };

  std::array<lod_instance_data, (usize)lod::detail_level::lod_detail_enum_size> lod_models;

  glm::vec3 instance_min{std::numeric_limits<f32>::max()}, instance_max{std::numeric_limits<f32>::min()};

  spacial_grid grid;

  u0
  distribute(std::array<lod_instance_data, (usize)lod::detail_level::lod_detail_enum_size> &array, i32 number_to_distribute,
             f32 shrink_factor)
  {
    int num_elements = array.size();
    int remaining    = number_to_distribute; // Remaining amount to distribute

    // Start from the smallest value and work towards the largest
    for (int i = 0; i < num_elements; ++i)
    {
      if (i == num_elements - 1)
      {
        // Last element takes the remaining amount to ensure the total adds up to number_to_distribute
        array[i].instance_count = remaining;
      }
      else
      {
        // Calculate the current value based on reverse shrink factor
        double factor           = std::pow(shrink_factor, num_elements - i - 1);
        array[i].instance_count = static_cast<int>(std::round(remaining * (1 - shrink_factor) * factor));
        remaining -= array[i].instance_count; // Subtract from the remaining amount
      }
    }
  }

  u0
  load_model_from_file(std::string_view _path)
  {
    for (usize i = 0; i != (usize)lod::detail_level::lod_detail_enum_size; ++i)
    {
      lod_models[i].model.load_from_file(_path, (lod::detail_level)i);
      lod_models[i].model.set_tex_lod_level((lod::detail_level)i);
    }
  }

  /*
  call this after putting in all of the instances and applying their transformations
  */
  u0
  initialize_culling_grid()
  {
    grid.initialize_with_divs(instance_min, instance_max, 5, this->instance_data_buffer);
  }

  u0
  set_instance_count(usize new_count)
  {
    if (new_count == instance_count)
    {
      return;
    }

    instance_count = new_count;
    instance_data_buffer.resize(new_count);
    instance_mappings.resize(new_count);
    translated_instance_data.resize(new_count);

    gpu_buffer.set_size(new_count * sizeof(glm::mat4));
    remap_buffer.set_size(new_count * sizeof(u16));

    /*
    when resizing model we also initialize the counts for the LOD scales
    */
    distribute(lod_models, instance_count, 0.8);

    usize base_index_accumulator = 0;
    for (usize i = 0; i != lod_models.size(); ++i)
    {
      auto &lod_model = lod_models[i];
      LOG(INFO) << "lod scale: " << magic_enum::enum_name((lod::detail_level)i);
      lod_model.model.set_instance_count(lod_model.instance_count);
      lod_model.model.set_base_instance_index(base_index_accumulator);
      base_index_accumulator += lod_model.instance_count;
      LOG(INFO) << "  instance count: " << lod_model.instance_count;
      LOG(INFO) << "  base index: " << lod_model.instace_base_index;
    }
  }

  u0
  remap_distance_based(const glm::vec3 &_location, const camera_frust &_frustum)
  {
    std::vector<std::pair<f32, u16>> distance_index_pair;
    distance_index_pair.reserve(instance_count);

    u32 rendered_meshes = 0;

    for (auto &grid_block : grid.blocks)
    {
      if (grid_block.is_in_frustum(_frustum))
      {
        grid_block.currently_active = true;
        rendered_meshes += grid_block.contained_instance_indices.size();
        for (auto index : grid_block.contained_instance_indices)
        {
          f32 dist = glm::distance(_location, instance_data_buffer[index].world_position);
          distance_index_pair.emplace_back(dist, index);
        }
      }
      else
      {
        grid_block.currently_active = false;
      }
    }

    // printf("rendering blocks: %d out of %d\n", rendered_meshes, grid.blocks.size());

    /*
    sort by farthest first since index 0 in lod_models = lowest LOD model;
    */
    std::sort(std::execution::par_unseq, distance_index_pair.begin(), distance_index_pair.end(),
              [&](const std::pair<f32, u16> &_left, const std::pair<f32, u16> &_right) { return _left.first < _right.first; });

    /*
    remove count of lowest LOD scale until temporary instance count matches what we actually want to render
    do not change the distribution. so that we can render more high res models even if we render less in total
    */

    i32 current_lod_level = (i32)lod::detail_level::lod_detail_potato;
    usize over_draw       = (usize)std::max(0, (i32)instance_count - (i32)rendered_meshes);
    /*
    @TODO: please optimize me
    */
    while (current_lod_level >= (i32)lod::detail_level::lod_detail_full /* 0 */)
    {
      auto &ite                     = lod_models[current_lod_level];
      const auto old_instance_count = (i32)ite.instance_count;
      const i32 new_instance_count  = (usize)std::max(0, (i32)ite.instance_count - (i32)over_draw);
      ite.model.set_instance_count(new_instance_count);
      over_draw = (usize)std::max(0, (i32)over_draw - (old_instance_count - new_instance_count));
      current_lod_level--;
    }

    // update the instance counts and base indices

    for (usize i = 0; i != distance_index_pair.size(); ++i)
    {
      instance_mappings[i] = distance_index_pair[i].second;
    }
  }

  u0
  update_mappings_on_gpu()
  {
    remap_buffer.buffer_data((u8 *)instance_mappings.data(), instance_mappings.size() * sizeof(u16));
  }

  u0
  buffer()
  {
    if (instance_count == 0)
    {
      return;
    }

    assert(instance_data_buffer.size() == instance_count);
    assert(translated_instance_data.size() == instance_count);

    gpu_buffer.buffer_data((u8 *)translated_instance_data.data(), sizeof(glm::mat4) * instance_count);
    remap_buffer.buffer_data((u8 *)instance_mappings.data(), instance_mappings.size() * sizeof(u16));
  }

  auto &
  get_instance_data()
  {
    return instance_data_buffer;
  }

  // TODO: possibly slow and needs some scaling optimizations as in threading or maybe a compute shader
  u0
  apply_translation_at(usize _index)
  {
    assert(_index < instance_data_buffer.size());
    auto &instance = instance_data_buffer.at(_index);

    const auto model_aabb_min = instance.world_position + lod_models[0].model.aabb.min * instance.world_scale;
    const auto model_aabb_max = instance.world_position + lod_models[0].model.aabb.max * instance.world_scale;

    instance_min.x = std::min(model_aabb_min.x, instance_min.x);
    instance_min.y = std::min(model_aabb_min.y, instance_min.y);
    instance_min.z = std::min(model_aabb_min.z, instance_min.z);

    instance_max.x = std::max(model_aabb_max.x, instance_max.x);
    instance_max.y = std::max(model_aabb_max.y, instance_max.y);
    instance_max.z = std::max(model_aabb_max.z, instance_max.z);

    glm::mat4 model = glm::mat4(1.0f);

    glm::vec3 rotation_axis = glm::cross(glm::vec3{0, 1, 0}, instance.world_rotation);
    float cos_angle         = glm::dot(glm::vec3{0, 1, 0}, instance.world_rotation);
    float angle             = glm::acos(cos_angle);

    model = glm::translate(model, instance.world_position);

    if (glm::length(rotation_axis) > 0.0001f)
    {
      model = glm::rotate(model, angle, rotation_axis);
      model = glm::rotate(model, (f32)rand(), {0, 1, 0});
    }

    model = glm::scale(model, instance.world_scale);

    glm::quat rot = glm::quat(glm::radians(instance.world_rotation));

    /*
    update the bbox for the render mesh once offline
    */

    instance.bounding_box.axes[0] = rot * lod_models[0].model.bounding_box.axes[0];
    instance.bounding_box.axes[1] = rot * lod_models[0].model.bounding_box.axes[1];
    instance.bounding_box.axes[2] = rot * lod_models[0].model.bounding_box.axes[2];

    instance.bounding_box.center = instance.world_position + lod_models[0].model.bounding_box.center * instance.world_scale;

    /*
    TODO: scaling issues persists here too
    */

    translated_instance_data.at(_index) = model;
    /*
    reset mappings
    */
    instance_mappings[_index] = (u16)_index;
  }

  u0
  reapply_all_translations()
  {
    for (usize i = 0; i != instance_data_buffer.size(); ++i)
    {
      apply_translation_at(i);
    }
  }

  u0
  draw(const render_camera_t &_camera)
  {
    if (instance_count == 0)
    {
      return;
    }

    assert(instance_data_buffer.size() == instance_count);
    assert(translated_instance_data.size() == instance_count);

    const auto frust = _camera.createFrustumFromCamera(1920.f / 1080.f, glm::radians(90.f), 0.1f, 10'000.f);

    remap_distance_based(_camera.vec_position, frust);
    update_mappings_on_gpu();

    remap_buffer.bind();
    gpu_buffer.bind();
    for (auto &model : lod_models)
    {
      model.model.draw(_camera);
    }
    gpu_buffer.unbind();
    remap_buffer.unbind();

    // grid.debug_draw(this->instance_data_buffer);
  }
};

} // namespace renderer