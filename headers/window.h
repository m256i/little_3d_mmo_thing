#pragma once

#include <iostream>
#include <string_view>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../common.h"

inline std::string
decodeDebugSource(GLenum source)
{
  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
    return "API";
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "Window System";
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    return "Shader Compiler";
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "Third Party";
  case GL_DEBUG_SOURCE_APPLICATION:
    return "Application";
  case GL_DEBUG_SOURCE_OTHER:
    return "Other";
  default:
    return "Unknown Source";
  }
}

inline std::string
decodeDebugType(GLenum type)
{
  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    return "Error";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "Deprecated Behavior";
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "Undefined Behavior";
  case GL_DEBUG_TYPE_PORTABILITY:
    return "Portability";
  case GL_DEBUG_TYPE_PERFORMANCE:
    return "Performance";
  case GL_DEBUG_TYPE_MARKER:
    return "Marker";
  case GL_DEBUG_TYPE_PUSH_GROUP:
    return "Push Group";
  case GL_DEBUG_TYPE_POP_GROUP:
    return "Pop Group";
  case GL_DEBUG_TYPE_OTHER:
    return "Other";
  default:
    return "Unknown Type";
  }
}

inline void GLAPIENTRY
glDebugCallbackFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  std::cerr << "OpenGL Debug Message:"
            << " Source: " << decodeDebugSource(source) << " Type: " << decodeDebugType(type) << " ID: " << id << " Severity: " << severity
            << " Message: " << message << std::endl;
}

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

    i32 monitor_count{};

    GLFWmonitor** monitor_pointer = glfwGetMonitors(&monitor_count);
    const GLFWvidmode* vmode      = glfwGetVideoMode(monitor_pointer[0]);

    window_width  = vmode->width;
    window_height = vmode->height;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window = glfwCreateWindow(window_width, window_height, _name.data(), *monitor_pointer, nullptr);

    if (!window) [[unlikely]]
    {
      std::puts("[logging]: failed to initialize Window!");
      glfwTerminate();
      return;
    }

    glfwWindowHint(GLFW_SAMPLES, 16);
    // glfwWindowHint(GLFW_DEPTH_BITS, 24); // Request a 24-bit depth buffer

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) [[unlikely]]
    {
      std::puts("[logging]: failed to initialize GLAD!");
      return;
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST | GL_DEPTH_BUFFER_BIT);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSwapInterval(0);

    glfwSetWindowPos(window, 0, 0);
    glfwShowWindow(window);

    glEnable(GL_BLEND);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glDebugCallbackFunc, 0);
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