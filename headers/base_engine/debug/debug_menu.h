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
  static constexpr i64 fps_graph_update_rate = 1; // in seconds
  std::deque<f32> fps_tracks{};

  std::array<char, 30> console_input_buff{};
  std::stringstream console_buffer;
  usize last_print_index = 0;
  std::streambuf* old    = nullptr;

  u0
  init_menu(GLFWwindow* _window)
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // redirect console out
    old = std::cout.rdbuf(console_buffer.rdbuf());
  }

  u0
  print_stdcout()
  {
    return;
    /* broken for now */
    std::string_view out_buf = console_buffer.view();

    if (out_buf.size() >= 1)
    {
      printf("%s", out_buf.substr(last_print_index, out_buf.size() - last_print_index).data());
    }

    console_buffer.seekg(0, std::ios::end);
    usize size = console_buffer.tellg();
    console_buffer.seekg(0, std::ios::beg);

    last_print_index += (size - last_print_index);
  }

  const ImVec2 console_size{700, 400};

  u0
  draw(GLFWwindow* _window, bool _is_open, f32 _delta_time)
  {

    if (draw_fps_counter)
    {
      if (!((i64)glfwGetTime() % fps_graph_update_rate))
      {
        static i64 last_update_time = 0;

        if ((i64)glfwGetTime() != last_update_time)
        {
          fps_tracks.push_back(1.f / _delta_time);
          if (fps_tracks.size() >= 20)
          {
            fps_tracks.pop_front();
          }
          last_update_time = (i64)glfwGetTime();
        }
      }
    }

    if (!_is_open)
    {
      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (_is_open)
    {
      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (_is_open)
    {
      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      bool open = true;

      if (ImGui::Begin("perf stats", &open))
      {
        if (draw_fps_counter)
        {
          ImGui::Text("fps: %f", 1.f / _delta_time);

          std::vector<f32> temp_fps_tracks;
          temp_fps_tracks.reserve(fps_tracks.size());

          for (const auto ite : fps_tracks)
          {
            temp_fps_tracks.push_back(ite);
          }

          ImGui::PlotLines("fps graph", temp_fps_tracks.data(), temp_fps_tracks.size(), 0, 0, 10, 5000, ImVec2(250, 100));
        }
        ImGui::End();
      }

      if (ImGui::Begin("console", nullptr, ImGuiWindowFlags_NoResize))
      {
        if (ImGui::InputText(" ", console_input_buff.data(), console_input_buff.size()))
        {
          std::fill(console_input_buff.begin(), console_input_buff.end(), '\0');
        }
        ImGui::SameLine();

        if (ImGui::Button("execute"))
        {
          std::fill(console_input_buff.begin(), console_input_buff.end(), '\0');
        }
        ImGui::Text("%s", console_buffer.view().data());
        ImGui::End();
      }

      ImGui::Render();
    }

    if (_is_open)
    {
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
  }

  bool draw_fps_counter = true;
};