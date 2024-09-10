#pragma once

#include <common.h>
#include <glm/glm.hpp>
#include <base_engine/debug/debug_overlay.h>
#include "static_world_model.h"

#include "core/ssbos.h"

struct instance_data
{
  glm::vec3 world_position{};
  glm::vec3 world_scale{1.f, 1.f, 1.f};
  glm::vec3 world_rotation{};
  /* later maybe add more per instance data */
};

struct instanced_static_world_model
{
  static_world_model_t base_model{"instance_model"};
  usize instance_count{0};

  std::vector<instance_data> instance_data_buffer{};
  std::vector<glm::mat4> translated_instance_data{};

  instanced_static_world_model(usize _binding_point) : binding_point(_binding_point), gpu_buffer(_binding_point) {}

  usize binding_point;
  core::ssbo_t gpu_buffer;

  u0
  load_model(std::string_view _path)
  {
    base_model.load_model(_path);
  }

  u0
  init_shader(std::string_view vs_path, std::string_view fs_path)
  {
    base_model.init_shader(vs_path, fs_path);
  }

  u0
  set_instance_count(usize new_count)
  {
    if (new_count == instance_count)
    {
      return;
    }

    instance_count = new_count;
    instance_data_buffer.resize(new_count);
    translated_instance_data.resize(new_count);
    gpu_buffer.set_size(new_count * sizeof(glm::mat4));
  }

  u0
  buffer()
  {
    if (instance_count == 0)
    {
      return;
    }

    assert(instance_data_buffer.size() == instance_count);
    assert(translated_instance_data.size() == instance_count);

    gpu_buffer.buffer_data((u8 *)translated_instance_data.data(), sizeof(glm::mat4) * instance_count);
  }

  auto &
  get_instance_data()
  {
    return instance_data_buffer;
  }

  // TODO: possibly slow and needs some scaling optimizations as in threading or maybe a compute shader
  u0
  apply_translation_at(usize _index)
  {
    assert(_index < instance_data_buffer.size());

    auto &instance = instance_data_buffer.at(_index);

    glm::mat4 model = glm::mat4(1.0f);

    glm::vec3 rotation_axis = glm::cross(glm::vec3{0, 1, 0}, instance.world_rotation);
    float cos_angle         = glm::dot(glm::vec3{0, 1, 0}, instance.world_rotation);
    float angle             = glm::acos(cos_angle);

    model = glm::translate(model, instance.world_position);

    if (glm::length(rotation_axis) > 0.0001f)
    {
      model = glm::rotate(model, angle, rotation_axis);
      model = glm::rotate(model, (f32)rand(), {0, 1, 0});
    }

    model = glm::scale(model, instance.world_scale);

    translated_instance_data.at(_index) = model;
  }

  u0
  reapply_all_translations()
  {
    for (usize i = 0; i != instance_data_buffer.size(); ++i)
    {
      apply_translation_at(i);
    }
  }

  u0
  draw(const glm::mat4 &_projection, const glm::mat4 &_view)
  {
    if (instance_count == 0)
    {
      return;
    }

    assert(instance_data_buffer.size() == instance_count);
    assert(translated_instance_data.size() == instance_count);

    gpu_buffer.bind();
    base_model.draw_no_model(_projection, _view, instance_count);
    gpu_buffer.unbind();
  }
};