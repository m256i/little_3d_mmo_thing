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

/* handles LOD levels internally for main mesh and also foliage */
/* possible things to think about in the future:
  - interactive foliage like trees and stones to cut down or chests (will hopefully be fixed by proper gameobj system)
  - what if we want different worldgen stuff like generating cave entrances using sbtractive method
 */

struct ground_mesh_chunk_t
{
  basic_shader_t shader{"ground_plane_funny"};

  renderer::image_tex_lod grass_texture{};
  renderer::image_tex_lod rock_texture{};

  f64 scale_xyz = 100.f;
  glm::vec3 world_position{0, 0, 0};

  u0
  load_shader()
  {
    shader.load_from_path("../world_mesh.vs", "../world_mesh.fs");
  }

  u32 vao, vbo, ebo;

  struct mesh_vert_instance
  {
    glm::vec3 vert{};
    glm::vec3 normal{};
  };

  std::vector<mesh_vert_instance> verts;
  std::vector<u32> indices{};

  constexpr static i32 grid_size = 50;

  u0
  initialize(usize _divs, i32 chunk_scale, debug_menu_t &debug_menu, glm::vec3 _coords = glm::vec3{0, 0, 0})
  {
    grass_texture.load("../data/texture/grass.png");
    rock_texture.load("../data/texture/rock.png");

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
            delete[] field;
            return;
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
    }

    for (const auto &index : mesh.indices)
    {
      indices.push_back(index);
    }

    // @TODO: abstract this stuff away into the /core/ library

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(mesh_vert_instance), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

    // coords
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)offsetof(mesh_vert_instance, vert));
    glEnableVertexAttribArray(0);

    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)offsetof(mesh_vert_instance, normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    delete[] field;
  }

  u0
  regenerate(i32 chunk_scale, debug_menu_t &debug_menu, glm::vec3 _coords = glm::vec3{0, 0, 0})
  {
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
            delete[] field;
            return;
          }
        }
      }
    }

    verts.clear();
    indices.clear();

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
    }

    for (const auto &index : mesh.indices)
    {
      indices.push_back(index);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(mesh_vert_instance), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

    // coords
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)offsetof(mesh_vert_instance, vert));
    glEnableVertexAttribArray(0);

    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)offsetof(mesh_vert_instance, normal));
    glEnableVertexAttribArray(1);

    delete[] field;
  }

  u0
  draw(auto display_w, auto display_h, auto &camera, lod::detail_level lod_level, u32 _col)
  {
    shader.use();

    static glm::mat4 _projection = glm::perspective(glm::radians(camera.fov), (float)display_w / (float)display_h, 0.1f, 10'000.f);
    glm::mat4 _view              = camera.get_view_matrix();
    glm::mat4 model              = glm::mat4(1.0f);

    // might be  * grid_size
    model = glm::translate(model, {world_position.x, world_position.y, world_position.z});
    model = glm::scale(model, {scale_xyz, scale_xyz, scale_xyz});

    // idk why i even rotate this
    // model = glm::rotate(model, glm::radians(90.f), glm::vec3{1.f, 0, 0});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);

    glBindVertexArray(vao);

    const auto sampler1_uni_loc = shader.get_uniform_location("texture_diffuse1");
    const auto sampler2_uni_loc = shader.get_uniform_location("texture_diffuse2");

    shader.set_uniform_i32_from_index(sampler1_uni_loc, 0);
    shader.set_uniform_i32_from_index(sampler2_uni_loc, 1);

    glActiveTexture(GL_TEXTURE0);
    grass_texture.bind(lod_level);

    glActiveTexture(GL_TEXTURE1);
    rock_texture.bind(lod_level);

    glDrawElements(GL_TRIANGLES, (i32)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    grass_texture.unbind();
    rock_texture.unbind();
  }

  u0
  destroy()
  {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
  }
};
