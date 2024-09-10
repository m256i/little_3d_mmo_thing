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
  // old = std::cout.rdbuf(console_buffer.rdbuf());
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

    if (ImGui::Begin("debug controls"))
    {
      for (auto& section : debug_wigdets)
      {
        if (ImGui::CollapsingHeader(section.first.c_str()))
        {
          for (auto& widget : section.second)
          {
            switch (widget.type)
            {
            case debug_widget_type::button:
            {
              if (ImGui::Button(widget.name.c_str()))
              {
                widget.callback(-1);
              }
              break;
            }
            case debug_widget_type::slider_f32:
            {
              ImGui::SliderFloat(widget.name.c_str(), &widget.curr_val_impl, widget.min_val, widget.max_val);
              widget.callback(widget.curr_val_impl);
              break;
            }
            case debug_widget_type::slider_i32:
            {
              i32 val = widget.curr_val_impl;
              ImGui::SliderInt(widget.name.c_str(), &val, widget.min_val, widget.max_val);
              widget.curr_val_impl = val;
              widget.callback(widget.curr_val_impl);
              break;
            }
            default:
            {
              break;
            }
            }
          }
        }
      }
    }
    ImGui::End();

    if (ImGui::Begin("console", nullptr, ImGuiWindowFlags_NoScrollbar))
    {
      ImGui::SetNextWindowSize({ImGui::GetContentRegionMax().x, ImGui::GetContentRegionMax().y - 20});
      ImGui::BeginChild("console##text", {0, 0}, true);
      {
        ImGui::Text("%s", console_buffer.str().c_str());
        ImGui::SetScrollHereY(1.f);
      }
      ImGui::EndChild();

      if (ImGui::InputText(" ", console_input_buff.data(), console_input_buff.size()))
      {
        std::fill(console_input_buff.begin(), console_input_buff.end(), '\0');
      }
      ImGui::SameLine();
      if (ImGui::Button("clear"))
      {
        console_buffer.str({});
        console_buffer.clear();
        // console_buffer.flush();
      }
      ImGui::SameLine();

      if (ImGui::Button("execute"))
      {
        std::fill(console_input_buff.begin(), console_input_buff.end(), '\0');
      }
      ImGui::SetScrollHereY(1.f);
    }
    ImGui::End();

    static i32 selected = 0;
    static std::string selected_name;

    if (ImGui::Begin("script editor"))
    {
      i32 counter = 0;
      for (auto& script_module : this->script_modules)
      {
        if (ImGui::Selectable(script_module.second.module_name.c_str(), selected == counter, 0, ImVec2{200, 16}))
        {
          selected_name = script_module.first;
          selected      = counter;
        }
        counter++;
      }

      ImGui::SameLine();

      if (ImGui::BeginChild("script editor", {0, 0}, true) && !selected_name.empty())
      {
        auto& script_module = this->script_modules.at(selected_name);

        if (!script_module.message.empty())
        {
          ImGui::Text("%s", script_module.message.c_str());
        }

        script_module.editor.Render("editor", {ImGui::GetWindowSize().x - 80, ImGui::GetWindowSize().y - 61});

        if (ImGui::Button("compile all"))
        {
          // clears all of the modules
          this->script_handler.reset_vm();

          /* always compile all script modules */
          for (auto& library : this->library_modules)
          {
            LOG(DEBUG) << "[scripting] : linking library module: " << library.first;
            /* reevalute the entire file */
            library.second.update_source();
            this->script_handler.compile_library_module(library.first.c_str(), library.second.source);
          }

          for (auto& recomp_script_module : this->script_modules)
          {
            auto text          = recomp_script_module.second.editor.GetText();
            const char* script = text.c_str();

            this->script_handler.compile_module(recomp_script_module.second.module_name.c_str(), script,
                                                recomp_script_module.second.required_class);

            auto current_module = script_handler.get_module(recomp_script_module.second.module_name.c_str());

            if (current_module && current_module->is_compiled)
            {
              bool success      = true;
              auto class_exists = current_module->check_if_class_exists(recomp_script_module.second.required_class.c_str());
              if (!class_exists)
              {
                LOG(DEBUG) << "[scripting] : " << recomp_script_module.second.module_name << " script requires '"
                           << recomp_script_module.second.required_class << "' class!";
                success = false;
              }
              auto func_exists = current_module->check_if_func_exists(script_module.required_method.c_str());
              if (!func_exists)
              {
                LOG(DEBUG) << "[scripting] : " << recomp_script_module.second.module_name << " script requires '"
                           << recomp_script_module.second.required_method << "' method!";
                success = false;
              }

              if (success)
              {
                LOG(DEBUG) << "[scripting] : succesfully compiled: " << current_module->name;
                recomp_script_module.second.cache_function = current_module->cache_function(
                    recomp_script_module.second.required_class.c_str(), recomp_script_module.second.required_method.c_str());
              }
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

    static i32 selected_lib = 0;
    static std::string selected_name_lib;

    if (ImGui::Begin("script api"))
    {
      i32 counter = 0;
      for (auto& library : this->library_modules)
      {
        if (ImGui::Selectable(library.second.module_name.c_str(), selected_lib == counter, 0, ImVec2{200, 16}))
        {
          selected_name_lib = library.first;
          selected_lib      = counter;
        }
        counter++;
      }

      ImGui::SameLine();

      if (ImGui::BeginChild("script api", {0, 0}, true) && !selected_name_lib.empty())
      {
        auto& library = this->library_modules.at(selected_name_lib);

        if (!library.message.empty())
        {
          ImGui::Text("%s", library.message.c_str());
        }

        library.editor.Render("editor", {ImGui::GetWindowSize().x - 80, ImGui::GetWindowSize().y - 61});
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