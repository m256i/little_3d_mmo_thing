#pragma once
#include "../../common.h"

#include <base_engine/renderer/model_renderer.h>
#include <base_engine/renderer/camera.h>

#include <GLFW/glfw3.h>

struct game_renderer_t
{
  model_renderer_t model_renderer{};
  render_camera_t game_camera{};

  i32 display_w{}, display_h{};

  void render();

  void update_frame_buffer(GLFWwindow* _window);
};