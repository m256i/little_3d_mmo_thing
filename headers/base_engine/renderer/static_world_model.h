#pragma once
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
  glm::vec3 vec_position{}, vec_scale{1.f};
  basic_shader_t shader;

  static_world_model_t(std::string_view _debug_name) : shader(_debug_name) {}

  void
  init_shader()
  {
    shader.load_from_path(basic_model_vs, basic_model_fs);
  }

  void
  load_model(std::string_view _path)
  {
    stbi_set_flip_vertically_on_load(true);
    draw_model.load_render_model(_path);
    stbi_set_flip_vertically_on_load(false);
  }

  void
  draw(const glm::mat4& _projection, const glm::mat4& _view) const
  {
    shader.use();
    shader.setMat4("projection", _projection);
    shader.setMat4("view", _view);

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, vec_position);
    model           = glm::scale(model, vec_scale);
    shader.setMat4("model", model);

    draw_model.draw(shader);
  }
};