#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>

#include <base_engine/renderer/shader.h>

#include "../../../common.h"
#include "base_engine/renderer/shader.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"

#define MC_IMPLEM_ENABLE

#include <include/MC/mc.h>
#include <include/SimplexNoise/SimplexNoise.h>

#include "../renderer/core/image_texture.h"

struct ground_plane_t
{
  basic_shader_t shader{"ground_plane_funny"};
  renderer::image_tex grass_texture{}, rock_texture{};

  u0
  load_shader()
  {
    shader.load_from_path("../world_mesh.vs", "../world_mesh.fs");
  }

  u32 vao, vbo, ebo;

  struct mc_tri_vert
  {
    glm::vec3 pos;
    glm::vec3 normal;
  };

  std::vector<f32> vertices{};
  std::vector<u32> indices{};

  std::vector<mc_tri_vert> vertices_;

  u0
  initialize(usize _divs)
  {
    SimplexNoise noise;

    const int n  = 50;
    float *field = new float[n * n * n];

    constexpr int static_height = 20;
    int ground_height           = static_height;

    // k = up-down
    /* gerenate ground surface */

    grass_texture.load("D:/git/WoW_Clone/data/texture/grass.png");
    rock_texture.load("D:/git/WoW_Clone/data/texture/rock.png");

    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
      {
        for (int k = 0; k < n; k++)
        {

          auto nv                 = noise.fractal(2, i * 0.101f, j * 0.101f, k * 0.101f);
          ground_height           = (static_height) + (noise.fractal(2, i * 0.01501f, j * 0.01501f) * 3);
          auto distance_to_ground = std::abs(ground_height - k) * 0.55;
          if (distance_to_ground <= 2)
          {
            nv -= 0.4;
          }
          if (distance_to_ground == 0)
          {
            nv = 0;
          }
          nv += 1.f - (1.f / distance_to_ground * 2);

          field[(k * n + j) * n + i] += nv;
        }
      }
    }

    MC::mcMesh mesh;
    MC::marching_cube(field, n, n, n, mesh);

    vertices.reserve(mesh.vertices.size() * 6);
    indices.reserve(mesh.indices.size());

    assert(mesh.vertices.size() == mesh.normals.size());

    for (usize i = 0; i < mesh.vertices.size(); i++)
    {
      auto &vertex = mesh.vertices[i];
      auto &normal = mesh.normals[i];

      // vertices_.push_back({{vertex.x, vertex.y, vertex.z}, {normal.x, normal.y, normal.z}});

      vertices.push_back(vertex.x);
      vertices.push_back(vertex.y);
      vertices.push_back(vertex.z);
      vertices.push_back(normal.x);
      vertices.push_back(normal.y);
      vertices.push_back(normal.z);
    }

    for (const auto &index : mesh.indices)
    {
      indices.push_back(index);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
  }

  u0
  draw(auto display_w, auto display_h, auto camera, u32 _col)
  {
    shader.use();

    static glm::mat4 _projection = glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 10'000.f);
    glm::mat4 _view              = camera->get_view_matrix();
    glm::mat4 model              = glm::mat4(1.0f);
    model                        = glm::translate(model, {1.f, 1.f, 1.f});
    model                        = glm::scale(model, {100.f, 100.f, 100.f});
    model                        = glm::rotate(model, glm::radians(90.f), glm::vec3{1.f, 0, 0});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);

    shader.setVec3("cursor", camera->vec_position);
    shader.setVec4("in_color", glm::vec4{50 / 255.f, 42 / 255.f, 43 / 255.f, 1.f});

    glBindVertexArray(vao);

    const auto sampler1_uni_loc = shader.get_uniform_location("texture_diffuse1");
    const auto sampler2_uni_loc = shader.get_uniform_location("texture_diffuse2");

    shader.set_uniform_i32_from_index(sampler1_uni_loc, 0);
    shader.set_uniform_i32_from_index(sampler2_uni_loc, 1);

    glActiveTexture(GL_TEXTURE0);
    grass_texture.bind();

    glActiveTexture(GL_TEXTURE1);
    rock_texture.bind();

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
