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

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __apple__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    i32 monitor_count{};

    GLFWmonitor** monitor_pointer = glfwGetMonitors(&monitor_count);
    const GLFWvidmode* vmode      = glfwGetVideoMode(monitor_pointer[0]);

    window_width  = vmode->width;
    window_height = vmode->height;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(window_width, window_height, _name.data(), *monitor_pointer, nullptr);

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

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSwapInterval(0);

    glfwSetWindowPos(window, 0, 0);
    glfwShowWindow(window);

    glEnable(GL_DEPTH_TEST | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
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
      glfwPollEvents();
      glfwSwapBuffers(window);
    }
    return *this;
  }

  game_window_t&
  init(auto _init_code)
  {
    _init_code(window);
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

inline game_window_t
create_window(std::string_view _name, bool _fullscreen) noexcept
{
  std::iostream::sync_with_stdio(false);
  return game_window_t(_name, _fullscreen);
}