#pragma once

#include <array>

#include <glm/vec3.hpp>

#include "../renderer/shader.h"
#include "../../common.h"
#include "../renderer/camera.h"
#include "../../logging/easylogging++.h"

#include "GLFW/glfw3.h"

static std::array<glm::vec3, 8> default_cube_verts{
    glm::vec3{-1, -1, 1}, // 0 bottom left
    glm::vec3{1, -1, 1},  // 1
    glm::vec3{-1, 1, 1},  // 2
    glm::vec3{1, 1, 1},   // 3

    glm::vec3{-1, -1, -1}, // 4
    glm::vec3{1, -1, -1},  // 5
    glm::vec3{-1, 1, -1},  // 6
    glm::vec3{1, 1, -1}    // 7 top right
};

static std::array<u32, 36> default_cube_indices = {
    0, 1, 3, 2, // front face
    4, 5, 7, 6, // back face
    0, 4, 6, 2, // left face
    1, 5, 7, 3, // right face
    2, 6, 7, 3, // top face
    0, 4, 5, 1  // bottom face
};

struct debug_overlay_t
{
  u0
  init(GLFWwindow* _window, const render_camera_t& _cam)
  {
    window = _window;
    camera = &_cam;
    shader.load_from_path("../debug.vs", "../debug.fs");
    glfwGetFramebufferSize(_window, &display_w, &display_h);
  }
  /*
  color works with 4 8bit segments in 32 bit uint:
  0x ff ff ff ff
     r  g  b  a
  */
  u0
  draw_AABB(glm::vec3 _min, glm::vec3 _max, u32 _col, bool _wireframe)
  {
    if (camera == nullptr)
    {
      LOG(INFO) << "[debug_overlay] : camera not initialized";
      return;
    }

    u32 vao, vbo, ebo;

    std::array<glm::vec3, 8> vertices{};
    std::memcpy(vertices.data(), default_cube_verts.data(), sizeof(vertices));
    std::array<u32, 36> indices{};
    std::memcpy(indices.data(), default_cube_indices.data(), sizeof(indices));

    glm::vec3 scalingFactors = glm::vec3{_max.x - _min.x, _max.y - _min.y, _max.z - _min.z} / 2.0f;
    glm::vec3 center         = glm::vec3{(_max.x + _min.x) / 2.f, (_max.y + _min.y) / 2.f, (_max.z + _min.z) / 2.f};

    for (int i = 0; i < 8; i++)
    {
      vertices[i] = center + (vertices[i] * scalingFactors);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    shader.use();

    glm::mat4 _projection = glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 _view       = camera->get_view_matrix();
    glm::mat4 model       = glm::mat4(1.0f);
    model                 = glm::translate(model, {1.f, 1.f, 1.f});
    model                 = glm::scale(model, {1.f, 1.f, 1.f});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);
    shader.setVec4("in_color", glm::vec4{(f32)((_col >> 24) & 0xff) / 255.f, (f32)((_col >> 16) & 0xff) / 255.f, (f32)((_col >> 8) & 0xff) / 255.f, (f32)((_col)&0xff) / 255.f});

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_QUADS, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
  }

  basic_shader_t shader{"debug_overlay_shader"};
  GLFWwindow* window;
  const render_camera_t* camera;
  i32 display_w, display_h;
};