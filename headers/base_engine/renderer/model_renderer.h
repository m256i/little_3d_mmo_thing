#pragma once

#include <vector>
#include <string_view>
#include <unordered_map>

#include <base_engine/renderer/static_world_model.h>

#include "../../common.h"

struct model_renderer_t
{
  std::unordered_map<std::string_view, static_world_model_t> static_world_models;

  void
  add_model(std::string_view _model_name, std::string_view _model_path)
  {
    static_world_models.emplace(_model_name, static_world_model_t(_model_name));
    static_world_models.at(_model_name).init_shader();
    static_world_models.at(_model_name).load_model(_model_path);
  }

  void
  render(const glm::mat4& _projection, const glm::mat4& _view)
  {
    for (const auto& prop : static_world_models)
    {
      prop.second.draw(_projection, _view);
    }
  }
};