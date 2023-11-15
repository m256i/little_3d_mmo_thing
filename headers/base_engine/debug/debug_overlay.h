#pragma once

#include <array>
#include <numeric>
#include <vector>

#include <glm/vec3.hpp>

#include <base_engine/renderer/shader.h>
#include <base_engine/renderer/camera.h>

#include <GLFW/glfw3.h>
#include <logging/easylogging++.h>

#include "../../common.h"

static constexpr std::array<glm::vec3, 8> default_cube_verts{
    glm::vec3{-1, -1, 1}, // 0 bottom left
    glm::vec3{1, -1, 1},  // 1
    glm::vec3{-1, 1, 1},  // 2
    glm::vec3{1, 1, 1},   // 3

    glm::vec3{-1, -1, -1}, // 4
    glm::vec3{1, -1, -1},  // 5
    glm::vec3{-1, 1, -1},  // 6
    glm::vec3{1, 1, -1}    // 7 top right
};

static constexpr std::array<u32, 36> default_cube_indices = {
    0, 1, 3, 2, // front face
    4, 5, 7, 6, // back face
    0, 4, 6, 2, // left face
    1, 5, 7, 3, // right face
    2, 6, 7, 3, // top face
    0, 4, 5, 1  // bottom face
};

struct debug_overlay_t
{
  static u0
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
  static u0
  draw_AABB(glm::vec3 _min, glm::vec3 _max, u32 _col, bool _wireframe)
  {
    if (camera == nullptr)
    {
      LOG(INFO) << "[debug_overlay] : camera not initialized";
      return;
    }

    u32 vao, vbo, ebo;

    std::array<glm::vec3, 8> vertices{default_cube_verts};
    std::array<u32, 36> indices{default_cube_indices};

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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    shader.use();

    glm::mat4 _projection =
        glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 _view = camera->get_view_matrix();
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, {1.f, 1.f, 1.f});
    model           = glm::scale(model, {1.f, 1.f, 1.f});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);
    shader.setVec4("in_color", glm::vec4{(f32)((_col >> 24) & 0xff) / 255.f, (f32)((_col >> 16) & 0xff) / 255.f,
                                         (f32)((_col >> 8) & 0xff) / 255.f, (f32)((_col)&0xff) / 255.f});

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

  static void
  draw_point(const glm::vec3& _location, u32 _col, bool _wireframe)
  {
    if (camera == nullptr)
    {
      LOG(INFO) << "[debug_overlay] : camera not initialized";
      return;
    }

    u32 vbo, vao;

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), (f32*)&_location, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    shader.use();

    glm::mat4 _projection =
        glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 _view = camera->get_view_matrix();
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, {1.f, 1.f, 1.f});
    model           = glm::scale(model, {1.f, 1.f, 1.f});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);
    shader.setVec4("in_color", glm::vec4{(f32)((_col >> 24) & 0xff) / 255.f, (f32)((_col >> 16) & 0xff) / 255.f,
                                         (f32)((_col >> 8) & 0xff) / 255.f, (f32)((_col)&0xff) / 255.f});

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, 1);

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  static void
  draw_vertex_buffer(f32* _vbuf, usize _vbuflen, u32* _ibuf, usize _ibuflen, u32 _col, bool _wireframe)
  {
    if (camera == nullptr)
    {
      LOG(INFO) << "[debug_overlay] : camera not initialized";
      return;
    }

    u32 vao, vbo, ebo;

    f32* vertices = _vbuf;
    u32* indices  = _ibuf;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, _vbuflen * sizeof(f32), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _ibuflen * sizeof(u32), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32) * 3, (void*)0);

    shader.use();

    glm::mat4 _projection =
        glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 _view = camera->get_view_matrix();
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, {1.f, 1.f, 1.f});
    model           = glm::scale(model, {1.f, 1.f, 1.f});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);
    shader.setVec4("in_color", glm::vec4{(f32)((_col >> 24) & 0xff) / 255.f, (f32)((_col >> 16) & 0xff) / 255.f,
                                         (f32)((_col >> 8) & 0xff) / 255.f, (f32)((_col)&0xff) / 255.f});

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_POINTS, static_cast<unsigned int>(_ibuflen), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
  }

  static void
  draw_plane(std::array<glm::vec3, 4> _points, u32 _col, bool _wireframe)
  {
    if (camera == nullptr)
    {
      LOG(INFO) << "[debug_overlay] : camera not initialized";
      return;
    }

    u32 vao, vbo, ebo;

    std::array<glm::vec3, 4> vertices{_points};
    std::array<u32, 4> indices{0, 1, 2, 3};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    shader.use();

    glm::mat4 _projection =
        glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 _view = camera->get_view_matrix();
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, {1.f, 1.f, 1.f});
    model           = glm::scale(model, {1.f, 1.f, 1.f});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);
    shader.setVec4("in_color", glm::vec4{(f32)((_col >> 24) & 0xff) / 255.f, (f32)((_col >> 16) & 0xff) / 255.f,
                                         (f32)((_col >> 8) & 0xff) / 255.f, (f32)((_col)&0xff) / 255.f});

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
  static void
  draw_line(std::array<glm::vec3, 2> _points, u32 _col, bool _wireframe)
  {
    if (camera == nullptr)
    {
      LOG(INFO) << "[debug_overlay] : camera not initialized";
      return;
    }

    u32 vao, vbo;

    std::array<glm::vec3, 2> vertices{_points};
    std::array<u32, 2> indices{0, 1};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    shader.use();

    glm::mat4 _projection =
        glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 _view = camera->get_view_matrix();
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, {1.f, 1.f, 1.f});
    model           = glm::scale(model, {1.f, 1.f, 1.f});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);
    shader.setVec4("in_color", glm::vec4{(f32)((_col >> 24) & 0xff) / 255.f, (f32)((_col >> 16) & 0xff) / 255.f,
                                         (f32)((_col >> 8) & 0xff) / 255.f, (f32)((_col)&0xff) / 255.f});

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
  }

  static void
  draw_triangle(std::array<glm::vec3, 3> _points, u32 _col, bool _wireframe)
  {
    if (camera == nullptr)
    {
      LOG(INFO) << "[debug_overlay] : camera not initialized";
      return;
    }

    u32 vbo, vao;

    std::array<glm::vec3, 3> vertices{_points};

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    shader.use();

    glm::mat4 _projection =
        glm::perspective(glm::radians(camera->fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 _view = camera->get_view_matrix();
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, {1.f, 1.f, 1.f});
    model           = glm::scale(model, {1.f, 1.f, 1.f});

    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    shader.setMat4("model", model);
    shader.setVec4("in_color", glm::vec4{(f32)((_col >> 24) & 0xff) / 255.f, (f32)((_col >> 16) & 0xff) / 255.f,
                                         (f32)((_col >> 8) & 0xff) / 255.f, (f32)((_col)&0xff) / 255.f});

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (_wireframe)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  static basic_shader_t shader;
  static GLFWwindow* window;
  static const render_camera_t* camera;
  static i32 display_w, display_h;
};

inline basic_shader_t debug_overlay_t::shader         = basic_shader_t{"debug_overlay_shader"};
inline GLFWwindow* debug_overlay_t::window            = nullptr;
inline const render_camera_t* debug_overlay_t::camera = nullptr;
inline i32 debug_overlay_t::display_w                 = 0;
inline i32 debug_overlay_t::display_h                 = 0;