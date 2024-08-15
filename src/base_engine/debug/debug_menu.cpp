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

#include "../../common.h"
#include <base_engine/debug/debug_menu.h>
#include <base_engine/debug/text_edit.h>

#include <include/wren/wren.hpp>

#include <include/imgui/imstb_textedit.h>
#include <base_engine/scripting/script_handler.h>

u0
debug_menu_t::init_menu(GLFWwindow* _window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(_window, true);
  ImGui_ImplOpenGL3_Init("#version 430");

  this->script_handler.init_vm();

  // redirect console out
  old = std::cout.rdbuf(console_buffer.rdbuf());
}

u0
debug_menu_t::print_stdcout()
{
  return;
  /* broken for now */
  std::string out_buf = console_buffer.str();

  if (out_buf.size() >= 1)
  {
    printf("%s", out_buf.substr(last_print_index, out_buf.size() - last_print_index).data());
  }

  console_buffer.seekg(0, std::ios::end);
  usize size = console_buffer.tellg();
  console_buffer.seekg(0, std::ios::beg);

  last_print_index += (size - last_print_index);
}

u0
debug_menu_t::draw(GLFWwindow* _window, bool _is_open, f32 _delta_time)
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
    }
    ImGui::End();

    if (ImGui::Begin("console", nullptr))
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
      ImGui::Text("%s", console_buffer.str().c_str());
      ImGui::SetScrollHereY(1.f);
    }
    ImGui::End();

    static i32 selected = 0;

    if (ImGui::Begin("script editor"))
    {
      i32 counter = 0;
      for (auto& script_module : this->script_modules)
      {
        if (ImGui::Selectable(script_module.module_name.c_str(), selected == counter, 0, ImVec2{200, 16}))
        {
          selected = counter;
        }
        counter++;
      }

      ImGui::SameLine();

      if (ImGui::BeginChild(1, {0, 0}, true))
      {
        auto& script_module = this->script_modules.at(selected);

        if (!script_module.message.empty())
        {
          ImGui::Text("%s", script_module.message.c_str());
        }

        script_module.editor.Render("", {ImGui::GetWindowSize().x - 80, ImGui::GetWindowSize().y - 61});

        if (ImGui::Button("compile"))
        {

          auto text          = script_module.editor.GetText();
          const char* script = text.c_str();

          this->script_handler.compile_module(script_module.module_name.c_str(), script);

          auto current_module = script_handler.get_module(script_module.module_name.c_str());

          if (current_module && current_module->is_compiled)
          {
            bool success      = true;
            auto class_exists = current_module->check_if_class_exists(script_module.required_class.c_str());
            if (!class_exists)
            {
              LOG(DEBUG) << "[scripting] : " << script_module.module_name << " script requires '" << script_module.required_class
                         << "' class!";
              success = false;
            }
            auto func_exists = current_module->check_if_func_exists(script_module.required_method.c_str());
            if (!func_exists)
            {
              LOG(DEBUG) << "[scripting] : " << script_module.module_name << " script requires '" << script_module.required_method
                         << "' method!";
              success = false;
            }

            if (success)
            {
              script_module.cache_function =
                  current_module->cache_function(script_module.required_class.c_str(), script_module.required_method.c_str());
            }
          }
        }

        ImGui::SameLine();

        if (ImGui::Button("save script"))
        {
          auto text                                                         = script_module.editor.GetText();
          const char* script                                                = text.c_str();
          this->script_handler.modules.at(script_module.module_name).source = script;

          this->script_handler.write_module_to_file(script_module.module_name, script_module.module_path);
          LOG(DEBUG) << "[scripting] : saving module '" << script_module.module_name << "' to file: '" << script_module.module_path << "'";
        }
      }
      ImGui::EndChild();
    }
    ImGui::End();

    // if (terrain_module && terrain_module->is_compiled && cache_function)
    // {
    //   std::cout << "function result cached function: " << terrain_module->call_function(cache_function, 20.f) << "\n";
    // }

    ImGui::Render();
  }

  if (_is_open)
  {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
}