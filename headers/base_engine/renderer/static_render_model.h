#pragma once

#include <common.h>
#include "core/render_pipeline.h"
#include "glm/ext/matrix_clip_space.hpp"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "render_primitives/bbox.h"

#include "lighting_system/baked_light.h"

namespace renderer
{
struct static_render_model
{
  glm::vec3 rotation{0, 0, 0}, position{0, 0, 0}, scale{1, 1, 1};

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

  primitives::obb bounding_box, translated_bbox;

  u0 load_from_file(std::string_view _path);

  u0
  update_location()
  {
    model = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3{1, 0, 0});
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3{0, 1, 0});
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3{0, 0, 1});

    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    glm::quat rot = glm::quat(glm::radians(rotation));

    translated_bbox.axes[0] = rot * bounding_box.axes[0];
    translated_bbox.axes[1] = rot * bounding_box.axes[1];
    translated_bbox.axes[2] = rot * bounding_box.axes[2];

    translated_bbox.center = position + bounding_box.center * scale;

    /*
    wrong
    */
    // translated_bbox.mins = bounding_box.mins * glm::vec3{scale.z, scale.z, scale.y};
    // translated_bbox.maxs = bounding_box.maxs * glm::vec3{scale.z, scale.z, scale.y};
  }

  u0
  set_lod_level(lod::detail_level _level)
  {
    for (auto &tex : pipeline.texture_cache)
    {
      tex.second.set_lod_level(_level);
    }
  }

  u0
  draw(auto &camera)
  {
    view = camera.get_view_matrix();
    pipeline.draw_function();
  }
};

struct static_render_model_lod
{
  glm::vec3 rotation{0, 0, 0}, position{0, 0, 0}, scale{1, 1, 1};

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
        },
        renderer::core::vertex_buffer::vertex_buffer_attribute
        {
          .name = "aLightMapTexCoords", .type = renderer::core::vertex_buffer::vbuff_attribute_type::type_f32, .count = 2
        },}},

      .textures = {
        {"texture_diffuse1", nullptr},
        {"texture_baked_light1", nullptr},
      },
      .shader   = {"static_render_model shader", "../shaders/basic_model.vs", "../shaders/basic_model.fs"},
      .fbo      = std::nullopt
  };
  // clang-format on

  glm::mat4 model, view, projection{glm::perspective(glm::radians(90.f), 1920.f / 1080.f, 0.1f, 10'000.f)};

  primitives::obb bounding_box;

  lighting::static_lighting::static_lighting_t lighting_thing{
      .light_sources{lighting::static_lighting::static_point_light_t{.position{10, 20, 5}, .color = 0xff0f0aff, .radius = 10.f}}};

  u0 load_from_file(std::string_view _path, lod::detail_level _detail_level);

  image_tex_lod atlas_debug_tex;

  u0
  update_location()
  {
    model = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3{1, 0, 0});
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3{0, 1, 0});
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3{0, 0, 1});

    model = glm::translate(model, position);
    model = glm::scale(model, scale);
  }

  u0
  set_lod_level(lod::detail_level _level)
  {
    for (auto &tex : pipeline.texture_cache)
    {
      tex.second.set_lod_level(_level);
    }
  }

  u0
  draw(auto &camera)
  {
    view = camera.get_view_matrix();
    pipeline.draw_function();

    glPointSize(5.f);
    debug_overlay_t::draw_point(glm::vec3{10, 20, 5}, 0xff0f0aff, true);
    glPointSize(1.f);
  }
};

struct instanced_static_render_model
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
      .shader   = {"instanced static_render_model shader", "../shaders/basic_model_instanced.vs", "../shaders/basic_model_instanced.fs"},
      .fbo      = std::nullopt
  };
  // clang-format on

  glm::mat4 view, projection{glm::perspective(glm::radians(90.f), 1920.f / 1080.f, 0.1f, 10'000.f)};

  primitives::obb bounding_box;

  u0 load_from_file(std::string_view _path);

  u0
  set_lod_level(lod::detail_level _level)
  {
    for (auto &tex : pipeline.texture_cache)
    {
      tex.second.set_lod_level(_level);
    }
  }

  u0
  draw(auto &camera)
  {
    view = camera.get_view_matrix();
    pipeline.draw_function();
  }
};

struct instanced_static_render_model_lod
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
      .shader   = {"instanced static_render_model shader", "../shaders/basic_model_instanced.vs", "../shaders/basic_model_instanced.fs"},
      .fbo      = std::nullopt
  };
  // clang-format on

  glm::mat4 view, projection{glm::perspective(glm::radians(90.f), 1920.f / 1080.f, 0.1f, 10'000.f)};

  primitives::obb bounding_box;
  primitives::aabb aabb;

  u0 load_from_file(std::string_view _path, lod::detail_level _detail_level);

  u0
  set_tex_lod_level(lod::detail_level _level)
  {
    for (auto &tex : pipeline.texture_cache)
    {
      tex.second.set_lod_level(_level);
    }
  }

  u0
  set_instance_count(usize _new_instance_count)
  {
    pipeline.set_instance_count(_new_instance_count);
  }

  u0
  set_base_instance_index(usize _new_index)
  {
    pipeline.set_base_instance_index(_new_index);
  }

  u0
  draw(auto &camera)
  {
    view = camera.get_view_matrix();
    pipeline.draw_function();
  }
};

} // namespace renderer