#pragma once
#include "../../common.h"
#include "camera.h"
#include "headers/base_engine/renderer/model_renderer.h"
#include "model_renderer.h"

#include "GLFW/glfw3.h"

struct game_renderer_t
{
  model_renderer_t model_renderer{};
  render_camera_t game_camera{};

  i32 display_w{}, display_h{};

  void
  render()
  {
    glm::mat4 projection = glm::perspective(glm::radians(game_camera.fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
    glm::mat4 view       = game_camera.get_view_matrix();

    model_renderer.render(projection, view);
  }

  void
  update_frame_buffer(GLFWwindow* _window)
  {
    glfwGetFramebufferSize(_window, &display_w, &display_h);
  }
};