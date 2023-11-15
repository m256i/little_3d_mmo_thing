#pragma once

#include <cstddef>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <deque>
#include <vector>
#include <algorithm>
#include <array>
#include <string_view>

#include "../../common.h"

struct debug_menu_t
{
  std::array<char, 30> console_input_buff{};
  
  static constexpr i64 fps_graph_update_rate = 1; // in seconds
  std::deque<f32> fps_tracks{};
  const ImVec2 console_size{700, 400};
  std::stringstream console_buffer;
  usize last_print_index = 0;
  std::streambuf* old    = nullptr;

  u0
  init_menu(GLFWwindow* _window);

  u0
  print_stdcout();


  u0
  draw(GLFWwindow* _window, bool _is_open, f32 _delta_time);

  bool draw_fps_counter = true;
};