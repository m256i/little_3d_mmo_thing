#pragma once

#include "base_engine/debug/text_edit.h"
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <deque>
#include <vector>
#include <array>

#include <common.h>

#include <base_engine/scripting/script_handler.h>

struct debug_menu_t
{
  std::array<char, 30> console_input_buff{};

  static constexpr i64 fps_graph_update_rate = 1; // in seconds
  std::deque<f32> fps_tracks{};
  const ImVec2 console_size{700, 400};
  std::stringstream console_buffer;
  usize last_print_index = 0;
  std::streambuf* old    = nullptr;

  scripting::engine_script_handler script_handler;

  struct script_module
  {
    std::string module_name;
    std::string module_path;

    std::string required_class;
    std::string required_method;

    // message to be displayed at top of editor
    std::string message;

    bool initialzed = false;
    TextEditor editor{};
    usize cache_function{};
    scripting::engine_script_handler::script_module* internal_module{};
  };

  u0
  add_user_script_module(const std::string& _module_name, const std::string& _path, const std::string& _required_class,
                         const std::string& _required_method, const std::string& _message)
  {
    script_modules.push_back(script_module{_module_name, _path, _required_class, _required_method, _message});

    script_handler.load_module_from_file(_module_name, _path);

    script_modules.back().internal_module = script_handler.get_module(_module_name);
    script_modules.back().editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Wren());
    script_modules.back().editor.SetShowWhitespaces(false);
    script_modules.back().editor.SetText(script_modules.back().internal_module->source);

    LOG(DEBUG) << "loaded: " << _module_name << " from path: " << _path;
    // @TODO: add shit
  }

  std::vector<script_module> script_modules{};

  u0 init_menu(GLFWwindow* _window);

  u0 print_stdcout();

  u0 draw(GLFWwindow* _window, bool _is_open, f32 _delta_time);

  bool draw_fps_counter = true;
};