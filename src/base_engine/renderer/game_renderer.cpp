#include <base_engine/renderer/game_renderer.h>

void
game_renderer_t::render()
{
  glm::mat4 projection =
      glm::perspective(glm::radians(game_camera.fov), (float)display_w / (float)display_h, 0.1f, 1000.0f);
  glm::mat4 view = game_camera.get_view_matrix();

  model_renderer.render(projection, view);
}

void
game_renderer_t::update_frame_buffer(GLFWwindow* _window)
{
  glfwGetFramebufferSize(_window, &display_w, &display_h);
}