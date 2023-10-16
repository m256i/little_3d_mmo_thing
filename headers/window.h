#pragma once
#include <concepts>
#include <iostream>
#include <string_view>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../common.h"

struct game_window_t
{
  game_window_t(std::string_view _name, bool _fullscreen) noexcept
  {
    if (!glfwInit()) [[unlikely]]
    {
      return;
    }

    if (_fullscreen)
    {
      glfwWindowHint(GLFW_DECORATED, GL_FALSE);
      glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    }

    i32 monitor_count{};

    GLFWmonitor** monitor_pointer = glfwGetMonitors(&monitor_count);
    const GLFWvidmode* vmode      = glfwGetVideoMode(monitor_pointer[0]);

    window_width  = vmode->width / 2;
    window_height = vmode->height / 2;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(window_width, window_height, _name.data(), nullptr, nullptr);

    if (!window) [[unlikely]]
    {
      std::puts("[logging]: failed to initialize Window!");
      glfwTerminate();
      return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) [[unlikely]]
    {
      std::puts("[logging]: failed to initialize GLAD!");
      return;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0);

    glEnable(GL_MULTISAMPLE);
    glfwDefaultWindowHints();
    glfwSetWindowPos(window, 0, 0);
    glfwShowWindow(window);
  }

  ~game_window_t() { glfwDestroyWindow(window); }

  game_window_t&
  register_callback(auto _glfw_callback_func, auto... _args)
  {
    _glfw_callback_func(window, _args...);
    return *this;
  }

  game_window_t&
  loop(auto _game_loop)
  {
    while (!glfwWindowShouldClose(window))
    {
      _game_loop(window);
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
    return *this;
  }

  i32
  stdexit()
  {
    return 0;
  }

private:
  GLFWwindow* window;
  u32 window_width, window_height;
};

static game_window_t
create_window(std::string_view _name, bool _fullscreen) noexcept
{
  std::iostream::sync_with_stdio(false);
  return game_window_t(_name, _fullscreen);
}
