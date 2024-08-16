#pragma once

#include <cmath>
#include <common.h>

#include "base_engine/renderer/core/lod.h"
#include "ground_mesh.h"
#include "../renderer/camera.h"

struct ground_mesh_system
{
  std::vector<ground_mesh_chunk_t> chunks{};

  usize num_chunks_sqrt = 2;
  i32 chunk_scale       = 100;

  u0
  init(debug_menu_t& debug_menu)
  {
    debug_menu.add_debug_widget("terrain", debug_menu_t::debug_widget_type::button, "regenerate",
                                [&]([[maybe_unused]] f64) { this->regenerate(debug_menu); });

    for (usize i = 0; i != num_chunks_sqrt; i++)
    {
      for (usize j = 0; j != num_chunks_sqrt; j++)
      {
        chunks.push_back({});
        // DO NOT CHANGE I,J & J,I  its messed up but it works
        chunks[(i * num_chunks_sqrt) + j].scale_xyz      = chunk_scale;
        chunks[(i * num_chunks_sqrt) + j].world_position = {j * chunk_scale, 0, i * chunk_scale};
        chunks[(i * num_chunks_sqrt) + j].initialize(0, chunk_scale, debug_menu, {i * chunk_scale, 0, j * chunk_scale});
        chunks[(i * num_chunks_sqrt) + j].load_shader();
      }
    }
  }

  u0
  regenerate(debug_menu_t& debug_menu)
  {
    for (usize i = 0; i != num_chunks_sqrt; i++)
    {
      for (usize j = 0; j != num_chunks_sqrt; j++)
      {
        // DO NOT CHANGE I,J & J,I  its messed up but it works
        chunks[(i * num_chunks_sqrt) + j].scale_xyz      = chunk_scale;
        chunks[(i * num_chunks_sqrt) + j].world_position = {j * chunk_scale, 0, i * chunk_scale};
        chunks[(i * num_chunks_sqrt) + j].regenerate(chunk_scale, debug_menu, {i * chunk_scale, 0, j * chunk_scale});
      }
    }
  }

  u0
  draw(auto display_w, auto display_h, render_camera_t& camera)
  {
    constexpr static auto vec3_dist = [](const glm::vec3& a, const glm::vec3& b) { return std::sqrt((b - a).x * (b - a).y * (b - a).z); };

    for (usize i = 0; i != num_chunks_sqrt; i++)
    {
      for (usize j = 0; j != num_chunks_sqrt; j++)
      {
        chunks[(i * num_chunks_sqrt) + j].draw(display_w, display_h, camera, lod::detail_level::lod_detail_high, 0);
      }
    }
  }
};