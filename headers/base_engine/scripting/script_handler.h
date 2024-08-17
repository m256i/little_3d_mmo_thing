#pragma once

#include <filesystem>
#include <include/wren/wren.hpp>
#include <cassert>
#include <common.h>
#include <optional>
#include <unordered_map>
#include <mutex>
#include "scripting_api.h"

namespace scripting
{

static inline void
writeFn(WrenVM* vm, const char* text)
{
  printf("%s", text);
}

static inline std::mutex print_mutex;

static inline void
errorFn(WrenVM* vm, WrenErrorType errorType, const char* module, const int line, const char* msg)
{
  std::lock_guard<std::mutex> lg(print_mutex);

  char msg_buf[256];

  switch (errorType)
  {
  case WREN_ERROR_COMPILE:
  {
    sprintf(msg_buf, "[%s line %d] [Error] %s\n", module, line, msg);
  }
  break;
  case WREN_ERROR_STACK_TRACE:
  {
    sprintf(msg_buf, "[%s line %d] in %s\n", module, line, msg);
  }
  break;
  case WREN_ERROR_RUNTIME:
  {
    sprintf(msg_buf, "[Runtime Error] %s\n", msg);
  }
  break;
  }

  std::cout << msg_buf;
}

struct engine_script_handler
{
  struct __attribute__((aligned(64))) runner_data
  {
    WrenVM* vm{nullptr};
    WrenConfiguration* config{nullptr};
  };

  runner_data vm;

  struct script_module
  {
    engine_script_handler* handler;
    std::string name, source;
    bool is_compiled = false;

    usize last_func_handle = 1;

    struct class_function_handle_pair
    {
      WrenHandle* function_handle;
      WrenHandle* class_handle;
    };

    std::unordered_map<usize, class_function_handle_pair> cached_function_handles{};

    u0
    mark_recompiled()
    {
      cached_function_handles.clear();
    }

    u0
    init(engine_script_handler* _handler, std::string_view _name)
    {
      assert(_handler);
      assert(_handler->vm.vm);

      handler = _handler;
      name    = _name;
    }

    bool
    check_if_class_exists(const std::string& _class_name /*string_view is not null-terminated*/)
    {
      if (!is_compiled)
      {
        return false;
      }

      if (_class_name.empty())
      {
        return false;
      }

      assert(handler);
      assert(handler->vm.vm);
      assert(!name.empty());
      assert(handler->modules.contains(name));

      auto vm = handler->vm.vm;

      wrenEnsureSlots(vm, 1);
      wrenGetVariable(vm, name.c_str(), _class_name.c_str(), 0);

      return wrenGetSlotType(vm, 0) == WREN_TYPE_UNKNOWN;
    }

    bool
    check_if_func_exists(const std::string& _func_name /*string_view is not null-terminated*/)
    {
      if (!is_compiled)
      {
        return false;
      }

      assert(handler);
      assert(handler->vm.vm);
      assert(!name.empty());

      auto vm = handler->vm.vm;

      for (i32 i = 0; i < vm->methodNames.count; i++)
      {
        // ew
        if (!std::strcmp(_func_name.c_str(), (*(vm->methodNames.data + i))->value))
        {
          return true;
        }
      }

      return false;
    }

    usize
    cache_function(const std::string& class_name, const std::string& func_sig)
    {
      if (!check_if_class_exists(class_name.data()))
      {
        std::cout << "[scripting] : no class named: " << class_name << " in module: " << name << " to cache\n";
        return 0;
      }

      if (!check_if_func_exists(func_sig))
      {
        std::cout << "[scripting] : no method named: " << func_sig << " in module: " << name << " to cache\n";
        return 0;
      }

      auto vm = handler->vm.vm;

      // put class name in first slot
      wrenEnsureSlots(vm, 1);
      wrenGetVariable(vm, name.c_str(), class_name.c_str(), 0);

      WrenHandle* classPtr = wrenGetSlotHandle(vm, 0);

      wrenSetSlotHandle(vm, 0, classPtr);

      WrenHandle* func_handle = wrenMakeCallHandle(vm, func_sig.data());

      cached_function_handles[++last_func_handle] = {.function_handle = func_handle, .class_handle = classPtr};

      return last_func_handle;
    }

    std::optional<double>
    call_function(usize function_handle, auto&&... args)
    {
      if (!is_compiled)
      {
        return std::nullopt;
      }

      assert(cached_function_handles.contains(function_handle));

      auto vm = handler->vm.vm;

      u32 arg_i = 1;

      wrenEnsureSlots(vm, sizeof...(args) + 1);
      (wrenSetSlotDouble(vm, arg_i++, args), ...);

      wrenSetSlotHandle(vm, 0, cached_function_handles[function_handle].class_handle);
      const auto result = wrenCall(vm, cached_function_handles[function_handle].function_handle);

      if (result != WREN_RESULT_SUCCESS)
      {
        return std::nullopt;
      }

      wrenEnsureSlots(vm, 1);
      double output = wrenGetSlotDouble(vm, 0);

      return output;
    }

    std::vector<WrenHandle> class_handles;
    std::vector<WrenHandle> function_handles;
  };

  u0
  init_vm()
  {
    WrenConfiguration* config = new WrenConfiguration;

    wrenInitConfiguration(config);
    config->writeFn             = &writeFn;
    config->errorFn             = &errorFn;
    config->bindForeignMethodFn = scripting::bindForeignMethod;

    auto new_vm = wrenNewVM(config);
    vm.vm       = new_vm;
    vm.config   = config;
  }

  u0
  close_vm()
  {
    // if (!thread_local_runners.contains(_tid))
    // {
    //   return;
    // }

    // wrenFreeVM(thread_local_runners.at(_tid).vm);
    // delete thread_local_runners.at(_tid).config;
  }

  u0
  reset_vm()
  {
    if (!vm.vm)
    {
      return;
    }

    wrenFreeVM(vm.vm);
    vm.vm = wrenNewVM(vm.config);
  }

  bool
  load_module_from_file(const std::string& module_name, const std::string& _file_path)
  {
    if (!std::filesystem::exists("../scripts/"))
    {
      std::filesystem::create_directory("../scripts/");
    }
    if (!std::filesystem::exists("../scripts/corelib/"))
    {
      std::filesystem::create_directory("../scripts/corelib/");
    }

    std::ifstream file(_file_path);
    if (!file.is_open())
    {
      LOG(DEBUG) << "module: " << module_name << " doesn't exist yet: creating new file";
      std::ofstream file(_file_path, std::ios::out | std::ios::trunc);
      if (!file.is_open())
      {
        LOG(DEBUG) << "can not create file: " << _file_path;
        return false;
      }
      file.close();
      create_module(module_name);
      return true;
    }
    else
    {
      auto str = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      create_module(module_name);
      compile_module(module_name, str);
    }
    return true;
  }

  u0
  write_module_to_file(const std::string& module_name, const std::string& _file_path)
  {
    if (!modules.contains(module_name))
    {
      LOG(DEBUG) << "can not write module '" << module_name << "' to file since it doesn't exist";
      return;
    }

    if (!std::filesystem::exists("../scripts/"))
    {
      std::filesystem::create_directory("../scripts/");
    }
    if (!std::filesystem::exists("../scripts/corelib/"))
    {
      std::filesystem::create_directory("../scripts/corelib/");
    }

    auto src = get_module(module_name)->source;

    std::ofstream file(_file_path, std::ios::out | std::ios::trunc);
    if (!file.is_open())
    {
      LOG(DEBUG) << "can not open file: " << _file_path;
      return;
    }
    file << src;
    if (file.fail())
    {
      LOG(DEBUG) << "can not write to file: " << _file_path;
      return;
    }
    file.close();
  }

  u0
  create_module(const std::string& module_name)
  {
    if (modules.contains(module_name))
    {
      return;
    }

    script_module new_module;
    new_module.init(this, module_name);
    modules.insert({module_name, new_module});

    LOG(DEBUG) << "created module: " << module_name;
  }

  u0
  compile_module(const std::string& module_name, const std::string& _source, const std::string& required_class = "")
  {
    if (!modules.contains(module_name))
    {
      LOG(DEBUG) << "[scripting] : module: " << module_name << " doesn't exist! aborting...\n";
      assert(false);
    }

    modules.at(module_name).source = _source;

    auto comp_result = wrenInterpret(vm.vm, module_name.c_str(), _source.c_str());

    if (comp_result == WREN_RESULT_SUCCESS)
    {
      modules.at(module_name).is_compiled = true;
      modules.at(module_name).mark_recompiled();

      if (modules.at(module_name).check_if_class_exists(required_class) && modules.at(module_name).check_if_func_exists("on_module_init()"))
      {
        std::cout << "[scripting] : (" << module_name << ") : on_module_init() called\n";
        usize temp_handle = modules.at(module_name).cache_function(required_class, "on_module_init()");
        modules.at(module_name).call_function(temp_handle);
      }
      else
      {
        std::cout << "[scripting] : (" << module_name << ") : on_module_init() skipped\n";
      }
    }
    else
    {
      modules.at(module_name).is_compiled = false;
    }
  }

  u0
  compile_library_module(const std::string& module_name, const std::string& _source)
  {
    if (!modules.contains(module_name))
    {
      LOG(DEBUG) << "[scripting] : library: " << module_name << " doesn't exist! aborting...\n";
      assert(false);
    }

    modules.at(module_name).source = _source;

    auto comp_result = wrenInterpret(vm.vm, module_name.c_str(), _source.c_str());

    LOG(DEBUG) << "successfully compiled: " << module_name;

    if (comp_result == WREN_RESULT_SUCCESS)
    {
      modules.at(module_name).is_compiled = true;
      modules.at(module_name).mark_recompiled();
    }
    else
    {
      modules.at(module_name).is_compiled = false;
    }
  }

  script_module*
  get_module(const std::string& name)
  {
    if (!modules.contains(name))
    {
      LOG(DEBUG) << "ERROR tried to load module: " << name << " that doesnt exist in scripting engine!";
      return nullptr;
    }

    return &modules.at(name);
  }

  std::unordered_map<std::string, script_module> modules;
};

} // namespace scripting