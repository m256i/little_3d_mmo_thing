#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"
#include "simple_shader.h"
#include "headers/window.h"

constinit f32 lastX = 1920.f / 2.0f;
constinit f32 lastY = 1080.f / 2.0f;

auto
mouse_callback([[maybe_unused]] GLFWwindow* window, double xpos_, double ypos_) -> u0
{
  lastX = (f32)xpos_;
  lastY = (f32)ypos_;
  printf("x: %f y: %f\n", lastX, lastY);
}

auto
main() -> i32
{
  return create_window("WoW Clone :D", false)
      .register_callback(glfwSetCursorPosCallback, mouse_callback)
      .loop(
          [](GLFWwindow* _window)
          {
            // main loop etc
            const simple_shader_t shad("./../vshader.vsha", "./../fshader.fsha");
            // hello please dont format weirdly
          })
      .stdexit();
}