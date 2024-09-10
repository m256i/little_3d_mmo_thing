#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>

#include <base_engine/renderer/shader.h>

#include "../../../common.h"
#include "base_engine/renderer/core/image_texture.h"
#include "base_engine/renderer/shader.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"

#define MC_IMPLEM_ENABLE

#include <include/MC/mc.h>
#include <include/SimplexNoise/SimplexNoise.h>

#include "../renderer/core/image_texture.h"

#include "../renderer/core/lod.h"

#include <meshoptimizer.h>

#include <base_engine/debug/debug_menu.h>

#include <base_engine/renderer/core/vertex_buffer.h>
#include <base_engine/renderer/core/render_pipeline.h>

/* handles LOD levels internally for main mesh and also foliage */
/* possible things to think about in the future:
  - interactive foliage like trees and stones to cut down or chests (will hopefully be fixed by proper gameobj system)
  - what if we want different worldgen stuff like generating cave entrances using sbtractive method
 */

/*
TODO: clean up using pseudo classes that dont actually do anything and just pass name and path for textures and shaders

*/
struct ground_mesh_chunk_t
{
  renderer::image_tex_lod grass_texture = renderer::image_tex_lod("grass_texture", "../data/texture/grass.png");
  renderer::image_tex_lod rock_texture  = renderer::image_tex_lod("rock_texture", "../data/texture/rock.png");

  f64 scale_xyz = 100.f;
  glm::vec3 world_position{0, 0, 0};

  struct mesh_vert_instance
  {
    glm::vec3 vert{};
    glm::vec3 normal{};
  };

  // clang-format off
  renderer::core::pipeline::render_pipeline pipeline 
  {
      .vbuf     = {{
        renderer::core::vertex_buffer::vertex_buffer_attribute
        {
          .name = "position", .type = renderer::core::vertex_buffer::vbuff_attribute_type::type_f32, .count = 3
        },
        renderer::core::vertex_buffer::vertex_buffer_attribute
        {
          .name = "normal", .type = renderer::core::vertex_buffer::vbuff_attribute_type::type_f32, .count = 3
        }}},
      .textures = 
        {
          {"grass_texture",  "../data/texture/grass.png"}, 
          {"rock_texture",  "../data/texture/rock.png"}
        },
      .shader   = {"stupi shader", "../shaders/world_mesh.vs", "../shaders/world_mesh.fs"},
      .fbo      = std::nullopt
  };
  // clang-format on

  std::vector<mesh_vert_instance> verts;
  std::vector<u32> indices{};

  constexpr static i32 grid_size = 50;

  glm::mat4 model, view, projection{glm::perspective(glm::radians(90.f), 1920.f / 1080.f, 0.1f, 10'000.f)};

  u0
  initialize(usize _divs, i32 chunk_scale, debug_menu_t &debug_menu, glm::vec3 _coords = glm::vec3{0, 0, 0})
  {
    pipeline.add_uniform_callack("model", [&]() { return model; });
    pipeline.add_uniform_callack("projection", [&]() { return projection; });
    pipeline.add_uniform_callack("view", [&]() { return view; });

    pipeline.initialize();

    scale_xyz      = chunk_scale;
    world_position = _coords;

    const f32 coord_scale = (f32)grid_size / chunk_scale;

    const i32 n  = grid_size + 1;
    float *field = new float[n * n * n];

    const i32 static_height = n / 2.f;
    i32 ground_height       = static_height;

    // k = up-down
    /* gerenate ground surface */

    for (i32 x = 0; x < n; ++x)
    {
      for (i32 y = 0; y < n; ++y)
      {
        for (i32 z = 0; z < n; ++z)
        {
          auto script_val = debug_menu.script_call("terrain", "terrain", "terrain_value(_,_,_)", (x + (_coords.x * coord_scale)),
                                                   (y + (_coords.y * coord_scale)), (z + (_coords.z * coord_scale)));

          if (script_val.has_value())
          {
            script_val.value()         = std::clamp(script_val.value(), 0.0, 1.0);
            field[(x * n + y) * n + z] = (script_val.value() - 0.5) * 2.0;
          }
          else
          {
            initialized = true;
            delete[] field;
            return; // this early return caused me alot of headaches
          }
        }
      }
    }

    MC::mcMesh mesh;
    MC::marching_cube(field, n, n, n, mesh);

    verts.reserve(mesh.vertices.size());
    indices.reserve(mesh.indices.size());

    assert(mesh.vertices.size() == mesh.normals.size());

    for (usize i = 0; i < mesh.vertices.size(); i++)
    {
      auto &vertex = mesh.vertices[i];
      auto &normal = mesh.normals[i];

      verts.push_back(mesh_vert_instance{glm::vec3{vertex.x, vertex.y, vertex.z}, glm::vec3{normal.x, normal.y, normal.z}});
      pipeline.push_back_vertex(mesh_vert_instance{glm::vec3{vertex.x, vertex.y, vertex.z}, glm::vec3{normal.x, normal.y, normal.z}});
    }

    for (const auto &index : mesh.indices)
    {
      indices.push_back(index);
      pipeline.push_back_index(index);
    }

    pipeline.setup_drawbuffer();
    initialized = true;

    delete[] field;
  }

  u0
  regenerate(i32 chunk_scale, debug_menu_t &debug_menu, glm::vec3 _coords = glm::vec3{0, 0, 0})
  {
    if (!pipeline.initialized)
    {
      pipeline.add_uniform_callack("model", [&]() { return model; });
      pipeline.add_uniform_callack("projection", [&]() { return projection; });
      pipeline.add_uniform_callack("view", [&]() { return view; });

      pipeline.initialize();
    }

    scale_xyz      = chunk_scale;
    world_position = _coords;

    const f32 coord_scale = (f32)grid_size / chunk_scale;

    const i32 n  = grid_size + 1;
    float *field = new float[n * n * n];

    const i32 static_height = n / 2.f;
    i32 ground_height       = static_height;

    // k = up-down
    /* gerenate ground surface */

    for (i32 x = 0; x < n; ++x)
    {
      for (i32 y = 0; y < n; ++y)
      {
        for (i32 z = 0; z < n; ++z)
        {
          auto script_val = debug_menu.script_call("terrain", "terrain", "terrain_value(_,_,_)", (x + (_coords.x * coord_scale)),
                                                   (y + (_coords.y * coord_scale)), (z + (_coords.z * coord_scale)));

          if (script_val.has_value()) [[likely]]
          {
            script_val.value()         = std::clamp(script_val.value(), 0.0, 1.0);
            field[(x * n + y) * n + z] = (script_val.value() - 0.5) * 2.0;
          }
          else
          {
            initialized = true;
            delete[] field;
            return;
          }
        }
      }
    }

    verts.clear();
    indices.clear();

    pipeline.clear_buffers();

    MC::mcMesh mesh;
    MC::marching_cube(field, n, n, n, mesh);

    verts.reserve(mesh.vertices.size());
    indices.reserve(mesh.indices.size());

    assert(mesh.vertices.size() == mesh.normals.size());

    for (usize i = 0; i < mesh.vertices.size(); i++)
    {
      auto &vertex = mesh.vertices[i];
      auto &normal = mesh.normals[i];

      vertex.x /= grid_size;
      vertex.y /= grid_size;
      vertex.z /= grid_size;

      verts.push_back(mesh_vert_instance{glm::vec3{vertex.x, vertex.y, vertex.z}, glm::vec3{normal.x, normal.y, normal.z}});
      pipeline.push_back_vertex(mesh_vert_instance{glm::vec3{vertex.x, vertex.y, vertex.z}, glm::vec3{normal.x, normal.y, normal.z}});
    }

    for (const auto &index : mesh.indices)
    {
      indices.push_back(index);
      pipeline.push_back_index(index);
    }

    pipeline.setup_drawbuffer();
    initialized = true;

    delete[] field;
  }

  u0
  draw(auto display_w, auto display_h, auto &camera, lod::detail_level lod_level, u32 _col)
  {
    assert(initialized);

    view = camera.get_view_matrix();

    model = glm::mat4(1.0f);
    model = glm::translate(model, {world_position.x, world_position.y, world_position.z});
    model = glm::scale(model, {scale_xyz, scale_xyz, scale_xyz});

    pipeline.draw_function();
  }

  u0
  destroy()
  {
  }

  bool initialized = false;
};
