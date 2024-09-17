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
#include <optional>
#include <unordered_map>
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

  struct lib_script_module
  {
    std::string module_name;
    std::string module_path;

    std::string source;

    // message to be displayed at top of editor
    std::string message;

    u0
    update_source()
    {
      std::ifstream file(module_path);
      if (!file.is_open()) return;
      source = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      editor.SetText(source);
    }

    TextEditor editor{};
    scripting::engine_script_handler::script_module* internal_module{};
  };

  enum class debug_widget_type
  {
    button,
    slider_f32,
    slider_i32,
    toggle,
  };

  struct debug_widget
  {
    debug_widget_type type;
    std::string name;
    std::function<void(f64)> callback;
    f32 min_val, max_val;
    f32 curr_val_impl = 0;
  };

  u0
  add_user_script_module(const std::string& _module_name, const std::string& _path, const std::string& _required_class,
                         const std::string& _required_method, const std::string& _message)
  {
    script_modules.insert({_module_name, script_module{_module_name, _path, _required_class, _required_method, _message}});
    script_handler.load_module_from_file(_module_name, _path);

    script_modules.at(_module_name).internal_module = script_handler.get_module(_module_name);
    script_modules.at(_module_name).editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Wren());
    script_modules.at(_module_name).editor.SetShowWhitespaces(false);
    script_modules.at(_module_name).editor.SetText(script_modules.at(_module_name).internal_module->source);

    LOG(DEBUG) << "loaded: " << _module_name << " from path: " << _path;
    // @TODO: add shit
  }

  u0
  add_lib_script_module(const std::string& _module_name, const std::string& _path, const std::string& _message)
  {
    library_modules.insert({_module_name, lib_script_module{_module_name, _path, "", _message}});
    script_handler.load_module_from_file(_module_name, _path);

    library_modules.at(_module_name).internal_module = script_handler.get_module(_module_name);
    library_modules.at(_module_name).source          = script_handler.get_module(_module_name)->source;

    library_modules.at(_module_name).editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Wren());
    library_modules.at(_module_name).editor.SetShowWhitespaces(false);
    library_modules.at(_module_name).editor.SetReadOnly(true);
    library_modules.at(_module_name).editor.SetText(library_modules.at(_module_name).internal_module->source);

    LOG(DEBUG) << "loaded library: " << _module_name << " from path: " << _path;
  }

  std::optional<double>
  script_call(std::string_view _module_name, std::string_view class_name, std::string function_name, auto&&... args)
  {
    std::string name{_module_name};
    if (!script_modules.contains(name))
    {
      return std::nullopt;
    }

    auto& smodule = script_modules.at(name);

    if (smodule.required_class != class_name) [[unlikely]]
    {
      return std::nullopt;
    }
    if (smodule.required_method != function_name) [[unlikely]]
    {
      return std::nullopt;
    }
    if (smodule.cache_function == 0) [[unlikely]]
    {
      return std::nullopt;
    }
    if (smodule.internal_module == nullptr) [[unlikely]]
    {
      return std::nullopt;
    }

    return smodule.internal_module->call_function(smodule.cache_function, args...);
  }

  std::unordered_map<std::string, script_module> script_modules{};
  std::unordered_map<std::string, lib_script_module> library_modules{};

  std::unordered_map<std::string, std::vector<debug_widget>> debug_wigdets{};

  u0
  add_debug_widget(const std::string& _section_name, debug_widget_type _type, const std::string& _widget_name,
                   const std::function<void(f64)>& _callback, f32 _min_val = -1.0, f32 _max_val = 1.0)
  {
    debug_wigdets[_section_name].push_back(
        debug_widget{.type = _type, .name = _widget_name, .callback = _callback, .min_val = _min_val, .max_val = _max_val});
  }

  u0 init_menu(GLFWwindow* _window);
  u0 print_stdcout();
  u0 draw(GLFWwindow* _window, bool _is_open, f32 _delta_time);

  bool draw_fps_counter = true;
};