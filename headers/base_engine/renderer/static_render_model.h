#pragma once

#include <common.h>
#include "core/render_pipeline.h"
#include "glm/ext/matrix_clip_space.hpp"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"

namespace renderer
{
struct static_render_model
{
  // clang-format off
  renderer::core::pipeline::render_pipeline pipeline 
  {
      .vbuf     = {{
        renderer::core::vertex_buffer::vertex_buffer_attribute
        {
          .name = "aPos", .type = renderer::core::vertex_buffer::vbuff_attribute_type::type_f32, .count = 3
        },
        renderer::core::vertex_buffer::vertex_buffer_attribute
        {
          .name = "aNormal", .type = renderer::core::vertex_buffer::vbuff_attribute_type::type_f32, .count = 3
        },
        renderer::core::vertex_buffer::vertex_buffer_attribute
        {
          .name = "aTexCoords", .type = renderer::core::vertex_buffer::vbuff_attribute_type::type_f32, .count = 2
        },}},

      .textures = {
        {"texture_diffuse1", nullptr}
      },
      .shader   = {"static_render_model shader", "../shaders/basic_model.vs", "../shaders/basic_model.fs"},
      .fbo      = std::nullopt
  };
  // clang-format on

  glm::mat4 model, view, projection{glm::perspective(glm::radians(90.f), 1920.f / 1080.f, 0.1f, 10'000.f)};

  u0 load_from_file(std::string_view _path);

  u0
  draw(auto &camera)
  {
    view = camera.get_view_matrix();

    model = glm::mat4(1.0f);
    model = glm::translate(model, {0, 0, 0});
    model = glm::scale(model, {1, 1, 1});

    pipeline.draw_function();
  }
};

} // namespace renderer