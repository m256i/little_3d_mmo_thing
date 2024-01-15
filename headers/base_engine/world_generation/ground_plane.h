#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>

#include <base_engine/renderer/shader.h>

#include "../../../common.h"
#include "GLFW/glfw3.h"
#include "base_engine/renderer/shader.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"

struct ground_plane_t
{
  basic_shader_t shader{"ground_plane_funny"};

  u0
  load_shader()
  {
    shader.load_from_path("../debug.vs", "../debug.fs", "../ground_plane.tesc", "../ground_plane.tese");
  }

  u32 vao, vbo, ebo;

  std::vector<f32> vertices{};
  std::vector<u32> indices{};

  u0
  initialize(usize _divs)
  {
    vertices = genNonSymPlaneUniform(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f),
                                     glm::vec3(-0.5f, 0.5f, 0.0f), _divs);

    indices = genPlaneIndices(_divs);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32) * 5, (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(f32) * 5, (void *)(5 * sizeof(f32)));
    glEnableVertexAttribArray(1);
  }

  u0
  draw(auto display_w, auto display_h, auto camera, u32 _col)
  {
    shader.use();

    static glm::mat4 _projection = glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 _view              = camera->get_view_matrix();
    glm::mat4 model              = glm::mat4(1.0f);
    model                        = glm::translate(model, {1.f, 1.f, 1.f});
    model                        = glm::scale(model, {700.f, 700.f, 700.f});
    model                        = glm::rotate(model, glm::radians(90.f), glm::vec3{1.f, 0, 0});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);

    shader.setVec3("cursor", camera->vec_position);
    shader.setVec4("in_color", glm::vec4{(f32)((_col >> 24) & 0xff) / 255.f, (f32)((_col >> 16) & 0xff) / 255.f,
                                         (f32)((_col >> 8) & 0xff) / 255.f, (f32)((_col) & 0xff) / 255.f});

    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_PATCHES, (i32)indices.size(), GL_UNSIGNED_INT, 0, 2);
    glBindVertexArray(0);
  }

  u0
  destroy()
  {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
  }

  // Plane indices for quad patches
  std::vector<u32>
  genPlaneIndices(usize div)
  {
    std::vector<u32> indices;

    for (usize row = 0; row < div; row++)
    {
      for (usize col = 0; col < div; col++)
      {
        usize index = row * (div + 1) + col;      // 3___2
        indices.push_back(index);                 //     |
        indices.push_back(index + 1);             //     |
        indices.push_back(index + (div + 1) + 1); //  ___|
        indices.push_back(index + (div + 1));     // 0   1
      }
    }

    return indices;
  }

  std::vector<f32>
  genNonSymPlaneUniform(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, usize div)
  {
    std::vector<f32> plane;

    glm::vec3 dir03 = (v3 - v0) / float(div);
    glm::vec3 dir12 = (v2 - v1) / float(div);

    // dir2 and dir3
    for (usize i = 0; i < div + 1; i++)
    {
      // dir1
      for (usize j = 0; j < div + 1; j++)
      {
        glm::vec3 acrossj = ((v1 + (f32)i * dir12) - (v0 + (f32)i * dir03)) / float(div);
        glm::vec3 crntVec = v0 + (f32)i * dir03 + (f32)j * acrossj;
        // Position
        plane.push_back(crntVec.x);
        plane.push_back(crntVec.y);
        plane.push_back(crntVec.z);
        // plane.push_back(0);
        //   Tex UV
        plane.push_back(float(j) / div);
        plane.push_back(float(i) / div);
      }
    }
    return plane;
  }
};
