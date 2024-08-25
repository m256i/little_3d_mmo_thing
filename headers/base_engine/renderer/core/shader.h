#pragma once

#include "base_engine/utils/zip.h"
#include <common.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <filesystem>

#include <base_engine/utils/fnv1a.h>

namespace renderer::core
{

namespace detail
{

struct internal_shader
{
  u32 id{};
  std::string_view debug_name{};

  internal_shader(std::string_view _debug_name) : debug_name(_debug_name) {};
  internal_shader(std::string_view _debug_name, std::string_view _vertex_path, std::string_view _frag_path,
                  std::string_view _tess_path = {}, std::string_view _tess_path_eval = {}, const char *_geo_path = nullptr)
      : debug_name(_debug_name)
  {
    if (!load_from_path(_vertex_path, _frag_path, _tess_path, _geo_path))
    {
      LOG(INFO) << "[shader] : error loading shader" << debug_name;
    }
    LOG(INFO) << "[shader] : " << debug_name << "compiled succesfully";
  }

  bool load_from_path(std::string_view _vertex_path, std::string_view _frag_path, std::string_view _tess_control_path = {},
                      std::string_view _tess_path_eval = {}, const char *_geo_path = nullptr);

  u0
  use() const
  {
    glUseProgram(id);
  }

  u32
  get_uniform_location(std::string_view _name)
  {
    return glGetUniformLocation(id, _name.data());
  }

  u0
  set_uniform_i32_from_index(u32 _uniform_id, i32 _value)
  {
    glUniform1i(_uniform_id, (int)_value);
  }

  u0
  setBool(std::string_view _name, bool _value) const
  {
    glUniform1i(glGetUniformLocation(id, _name.data()), (int)_value);
  }

  u0
  setInt(std::string_view _name, int _value) const
  {
    glUniform1i(glGetUniformLocation(id, _name.data()), _value);
  }

  u0
  setFloat(std::string_view _name, float _value) const
  {
    glUniform1f(glGetUniformLocation(id, _name.data()), _value);
  }

  u0
  setVec2(std::string_view _name, const glm::vec2 &_value) const
  {
    glUniform2fv(glGetUniformLocation(id, _name.data()), 1, &_value[0]);
  }

  u0
  setVec2(std::string_view _name, float _x, float _y) const
  {
    glUniform2f(glGetUniformLocation(id, _name.data()), _x, _y);
  }

  u0
  setVec3(std::string_view _name, const glm::vec3 &_value) const
  {
    glUniform3fv(glGetUniformLocation(id, _name.data()), 1, &_value[0]);
  }

  u0
  setVec3(std::string_view _name, float _x, float _y, float _z) const
  {
    glUniform3f(glGetUniformLocation(id, _name.data()), _x, _y, _z);
  }

  u0
  setVec4(std::string_view _name, const glm::vec4 &_value) const
  {
    glUniform4fv(glGetUniformLocation(id, _name.data()), 1, &_value[0]);
  }

  u0
  setVec4(std::string_view _name, float _x, float _y, float _z, float _w)
  {
    glUniform4f(glGetUniformLocation(id, _name.data()), _x, _y, _z, _w);
  }

  u0
  setMat2(std::string_view _name, const glm::mat2 &_mat) const
  {
    glUniformMatrix2fv(glGetUniformLocation(id, _name.data()), 1, GL_FALSE, &_mat[0][0]);
  }

  u0
  setMat3(std::string_view _name, const glm::mat3 &_mat) const
  {
    glUniformMatrix3fv(glGetUniformLocation(id, _name.data()), 1, GL_FALSE, &_mat[0][0]);
  }

  u0
  setMat4(std::string_view _name, const glm::mat4 &_mat) const
  {
    glUniformMatrix4fv(glGetUniformLocation(id, _name.data()), 1, GL_FALSE, &_mat[0][0]);
  }

  bool
  check_shadercomp_errors(u32 _shader, std::string_view _type)
  {
    i32 success;
    GLchar out_log[1024];
    if (_type != "PROGRAM")
    {
      glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(_shader, 1024, NULL, out_log);
        LOG(INFO) << "[shader] : error compiling shader: " << _type << "\n" << out_log;
        return false;
      }
    }
    else
    {
      glGetProgramiv(_shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(_shader, 1024, NULL, out_log);
        LOG(INFO) << "[shader] : error compiling shader: " << _type << "\n" << out_log;
        return false;
      }
    }
    return true;
  }
};
} // namespace detail

template <ct_string TName, typename TType>
struct shader_input
{
  static constexpr auto name = TName.to_view();
  using type                 = TType;
};

template <ct_string TName, typename TType>
struct shader_output
{
  static constexpr auto name = TName.to_view();
  using type                 = TType;
};

template <ct_string TShaderName, ct_string TShaderPath, typename... TShaderInOutNodes>
struct shader
{
private:
  template <typename T>
  struct is_shader_input : std::false_type
  {
  };

  template <ct_string TName, typename TType>
  struct is_shader_input<shader_input<TName, TType>> : std::true_type
  {
  };

  template <typename T>
  struct is_shader_output : std::false_type
  {
  };

  template <ct_string TName, typename TType>
  struct is_shader_output<shader_output<TName, TType>> : std::true_type
  {
  };

  template <template <typename> class TPredicate, typename... Ts>
  struct filter_types;

  template <template <typename> class TPredicate>
  struct filter_types<TPredicate>
  {
    using type = std::tuple<>;
  };

  template <template <typename> class TPredicate, typename T, typename... Ts>
  struct filter_types<TPredicate, T, Ts...>
  {
    using type = std::conditional_t<TPredicate<T>::value,
                                    decltype(std::tuple_cat(std::tuple<T>{}, typename filter_types<TPredicate, Ts...>::type{})),
                                    typename filter_types<TPredicate, Ts...>::type>;
  };

  // Convenience alias to get the filtered tuple
  template <template <typename> class TPredicate, typename... Ts>
  using filter_types_t = typename filter_types<TPredicate, Ts...>::type;

public:
  static constexpr auto shader_inputs  = filter_types_t<is_shader_input, TShaderInOutNodes...>{};
  static constexpr auto shader_outputs = filter_types_t<is_shader_output, TShaderInOutNodes...>{};

  detail::internal_shader internal_shader{TShaderName.to_view()};

  bool has_vertex = false, has_fragment = false, has_tesc = false, has_tese = false;

  bool initialized = false;

  bool
  setup()
  {
    const std::string shader_name_base = nullterminate_view(TShaderPath.to_view());
    std::string shader_real_name_base  = shader_name_base.substr(shader_name_base.find_last_of('/') + 1, shader_name_base.length());
    const std::string dir              = std::string(shader_name_base).substr(0, shader_name_base.find_last_of('/'));

    LOG(DEBUG) << shader_name_base;
    LOG(DEBUG) << dir;

    if (shader_name_base == dir)
    {
      LOG(DEBUG) << "could not extract dir from shader name: " << shader_name_base;
      assert(false);
    }

    if (!std::filesystem::exists(dir))
    {
      LOG(DEBUG) << "directory doesn't exist: " << dir;
      assert(false);
    }

    std::string vertex_name, fragment_name, tesc_name, tese_name;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(dir))
    {
      const auto &name           = std::string(entry.path().string());
      const std::string filename = name.substr(name.find_last_of('\\') + 1, std::string::npos) + "\0";

      /*
      ugly string concatination
      */
      const auto _vertex_name = std::string(shader_real_name_base) + ".vs";
      const auto _frag_name   = std::string(shader_real_name_base) + ".fs";
      const auto _tesc_name   = std::string(shader_real_name_base) + ".tesc";
      const auto _tese_name   = std::string(shader_real_name_base) + ".tese";

      if (filename == _vertex_name)
      {
        has_vertex  = true;
        vertex_name = name;
      }
      else if (filename == _frag_name)
      {
        has_fragment  = true;
        fragment_name = name;
      }
      else if (filename == _tesc_name)
      {
        has_tesc  = true;
        tesc_name = name;
      }
      else if (filename == _tese_name)
      {
        has_tese  = true;
        tese_name = name;
      }
    }

    std::cout << "has vertex: " << has_vertex << " has fragment: " << has_fragment << "\n";

    internal_shader.load_from_path(vertex_name, fragment_name, tesc_name, tese_name);

    char name[256];
    i32 name_length = 0;

    int ioCount;
    glGetProgramiv(internal_shader.id, GL_ACTIVE_ATTRIBUTES, &ioCount);

    std::cout << "ioCount" << ioCount << "\n";

    for (int i = 0; i < ioCount; i++)
    {
      int size;
      GLenum type;
      glGetActiveAttrib(internal_shader.id, i, 256, &name_length, &size, &type, name);
      std::cout << "found attribite: " << name << " size: " << size << " type: " << type << "\n";
    }

    GLuint program = internal_shader.id; // Your shader program ID
    GLint numOutputs;

    // Query number of outputs
    glGetProgramInterfaceiv(program, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &numOutputs);

    // To get detailed information about each output:
    GLenum properties[] = {GL_NAME_LENGTH, GL_LOCATION, GL_TYPE};

    for (int i = 0; i < numOutputs; ++i)
    {
      GLint results[3];
      glGetProgramResourceiv(program, GL_PROGRAM_OUTPUT, i, 3, properties, 3, nullptr, results);

      // Get the name of the output variable
      std::vector<char> nameData(results[0]);
      glGetProgramResourceName(program, GL_PROGRAM_OUTPUT, i, nameData.size(), nullptr, &nameData[0]);
      std::string name(nameData.begin(), nameData.end() - 1);

      GLint location = results[1];
      GLenum type    = results[2];

      std::cout << "found output: " << name << " location: " << location << " type: " << type << "\n";

      // Now, name, location, and type contain the information about the output variable
    }

    initialized = true;

    return true;
  }

  u0
  bind()
  {
    internal_shader.use();
  }
};

} // namespace renderer::core