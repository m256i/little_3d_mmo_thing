#pragma once
#include "core/lod.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "shader.h"
#include "model.h"
#include <glm/vec3.hpp>
#include <string_view>

static constexpr std::string_view basic_model_vs{"../basic_model.vs"};
static constexpr std::string_view basic_model_fs{"../basic_model.fs"};

struct static_world_model_t
{
  render_model_t draw_model{};
  glm::vec3 vec_position{}, vec_scale{1.f}, vec_rotation{};
  basic_shader_t shader;

  static_world_model_t(std::string_view _debug_name) : shader(_debug_name) {}

  void
  init_shader(std::string_view vs_path = "../basic_model.vs", std::string_view fs_path = "../basic_model.fs")
  {
    shader.load_from_path(vs_path, fs_path);
  }

  void
  load_model(std::string_view _path)
  {
    stbi_set_flip_vertically_on_load(true);
    draw_model.load_render_model(_path);
    stbi_set_flip_vertically_on_load(false);
  }

  void
  draw(const glm::mat4& _projection, const glm::mat4& _view, usize instance_count = 1) const
  {
    shader.use();
    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::rotate(model, vec_rotation.x, glm::vec3{1, 0, 0});
    model = glm::rotate(model, vec_rotation.y, glm::vec3{0, 1, 0});
    model = glm::rotate(model, vec_rotation.z, glm::vec3{0, 0, 1});

    model = glm::translate(model, vec_position);
    model = glm::scale(model, vec_scale);

    shader.setMat4("model", model);
    draw_model.draw(shader, instance_count);
  }

  void
  draw_no_model(const glm::mat4& _projection, const glm::mat4& _view, usize instance_count = 1) const
  {
    shader.use();
    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    draw_model.draw(shader, instance_count);
  }
};

struct lod_static_world_model_t
{
  lod_render_model_t draw_model{};
  glm::vec3 vec_position{}, vec_scale{1.f}, vec_rotation{};
  basic_shader_t shader;

  lod_static_world_model_t(std::string_view _debug_name) : shader(_debug_name) {}

  void
  init_shader(std::string_view vs_path = "../basic_model.vs", std::string_view fs_path = "../basic_model.fs")
  {
    shader.load_from_path(vs_path, fs_path);
  }

  void
  load_model(std::string_view _path)
  {
    stbi_set_flip_vertically_on_load(true);
    draw_model.load_render_model(_path);
    stbi_set_flip_vertically_on_load(false);
  }

  void
  draw(const glm::mat4& _projection, const glm::mat4& _view, lod::detail_level lod_level, usize instance_count = 1) const
  {
    shader.use();
    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, vec_position);
    model           = glm::scale(model, vec_scale);

    model = glm::rotate(model, vec_rotation.x, glm::vec3{1, 0, 0});
    model = glm::rotate(model, vec_rotation.y, glm::vec3{0, 1, 0});
    model = glm::rotate(model, vec_rotation.z, glm::vec3{0, 0, 1});

    shader.setMat4("model", model);
    draw_model.draw(shader, lod_level, instance_count);
  }

  void
  draw_no_model(const glm::mat4& _projection, const glm::mat4& _view, lod::detail_level lod_level, usize instance_count = 1) const
  {
    shader.use();
    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);
    draw_model.draw(shader, lod_level, instance_count);
  }
};