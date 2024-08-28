#pragma once

#include "gl_type_translations.h"
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

namespace detail
{
struct impl_unique_shader_tag_type
{
};
} // namespace detail

template <ct_string TName>
struct texture2d_shader_input
{
  static constexpr auto name = TName.to_view();
  /*
  type used since we have checks in the shader setup code
  */
  using type = detail::impl_unique_shader_tag_type;
};

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

template <ct_string TName, typename TType>
struct shader_uniform
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

  template <typename T>
  struct is_shader_uniform : std::false_type
  {
  };

  template <ct_string TName, typename TType>
  struct is_shader_uniform<shader_uniform<TName, TType>> : std::true_type
  {
  };

  template <typename T>
  struct is_texture_input : std::false_type
  {
  };

  template <ct_string TName>
  struct is_texture_input<texture2d_shader_input<TName>> : std::true_type
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

  template <typename TTuple>
  static consteval bool
  tuple_contains_texture()
  {
    bool out = false;
    for_constexpr<0, std::tuple_size_v<TTuple>, 1>(
        [&](auto ite)
        {
          using current_type = std::tuple_element_t<ite, TTuple>;
          if (std::is_same_v<typename current_type::type, detail::impl_unique_shader_tag_type>)
          {
            out = true;
          }
        });
    return out;
  }

public:
  static constexpr auto shader_inputs  = filter_types_t<is_shader_input, TShaderInOutNodes...>{};
  static constexpr auto shader_outputs = filter_types_t<is_shader_output, TShaderInOutNodes...>{};
  static constexpr auto shader_uniforms =
      std::tuple_cat(filter_types_t<is_shader_uniform, TShaderInOutNodes...>{}, filter_types_t<is_texture_input, TShaderInOutNodes...>{});

  detail::internal_shader internal_shader{TShaderName.to_view()};

  bool has_vertex = false, has_fragment = false, has_tesc = false, has_tese = false;
  bool initialized = false;

  /*
    @TODO: code cleanup some day later. this works but its really ugly
  */
  bool
  setup()
  {
    const std::string shader_name_base = nullterminate_view(TShaderPath.to_view());
    std::string shader_real_name_base  = shader_name_base.substr(shader_name_base.find_last_of('/') + 1, shader_name_base.length());
    const std::string dir              = std::string(shader_name_base).substr(0, shader_name_base.find_last_of('/'));

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

    if (!internal_shader.load_from_path(vertex_name, fragment_name, tesc_name, tese_name))
    {
      return false;
    }

    {
      char name[129];
      i32 name_length = 0;

      i32 io_count{};
      glGetProgramiv(internal_shader.id, GL_ACTIVE_ATTRIBUTES, &io_count);

      std::vector<std::pair<std::string, GLenum>> found_inputs{};
      found_inputs.reserve(io_count);

      for (i32 i = 0; i < io_count; i++)
      {
        i32 size;
        GLenum type;
        glGetActiveAttrib(internal_shader.id, i, 256, &name_length, &size, &type, name);
        found_inputs.push_back({std::string(name), type});
      }

      bool not_found = false, wrong_type = false;
      std::string_view not_found_name;
      std::string wrong_type_message;

      /*
      check if the c++ code requires inputs that are not present in the compiled shader
      */
      for_constexpr<0, std::tuple_size_v<decltype(shader_inputs)>, 1>(
          [&](auto ite)
          {
            if (not_found || wrong_type)
            {
              return;
            }

            bool current_found = false, current_right_type = false;
            GLenum _wrong_type               = 0;
            constexpr static auto shader_ipt = std::get<ite>(shader_inputs);

            if constexpr (std::is_same_v<typename decltype(shader_ipt)::type, detail::impl_unique_shader_tag_type>)
            {
              return;
            }
            else
            {
              for (const auto &_ipt_name : found_inputs)
              {
                if (shader_ipt.name == _ipt_name.first)
                {
                  current_found = true;
                  if (glsl_type_translation<typename decltype(shader_ipt)::type>::gl_type == _ipt_name.second)
                  {
                    current_right_type = true;
                  }
                  else
                  {
                    _wrong_type = _ipt_name.second;
                  }
                }
              }

              if (!current_found)
              {
                not_found_name = shader_ipt.name;
                not_found      = true;
              }

              if (!current_right_type)
              {
                wrong_type_message =
                    "attribute expteced type: " + std::string(glsl_type_name_map<typename decltype(shader_ipt)::type>::value) +
                    " got: " + std::string(glenum_type_to_strview(_wrong_type));
                wrong_type = true;
              }
            }
          });

      /*
       check if there are inputs declared and used in the shader file that the c++ code doesnt know about
       */
      for (const auto &_ipt_name : found_inputs)
      {
        bool current_needed = false;
        for_constexpr<0, std::tuple_size_v<decltype(shader_inputs)>, 1>(
            [&](auto ite)
            {
              constexpr static auto shader_ipt = std::get<ite>(shader_inputs);
              if constexpr (std::is_same_v<typename decltype(shader_ipt)::type, detail::impl_unique_shader_tag_type>)
              {
                return;
              }
              else
              {
                if (shader_ipt.name == _ipt_name.first)
                {
                  current_needed = true;
                }
              }
            });

        if (!current_needed)
        {
          LOG(INFO) << "shader attribute: " << _ipt_name.first << " is declared in shader file but not in c++ class";
          assert(false);
        }
      }

      if (not_found)
      {
        LOG(INFO) << "could not find required input in shader file: " << not_found_name << " in shader: " << vertex_name;
        assert(false);
      }

      if (wrong_type)
      {
        LOG(INFO) << wrong_type_message << " in shader: " << vertex_name;
        assert(false);
      }
    }

    {
      const GLuint program = internal_shader.id;
      GLint num_outputs;

      glGetProgramInterfaceiv(program, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &num_outputs);
      std::array properties{(GLenum)GL_NAME_LENGTH, (GLenum)GL_TYPE};

      std::vector<std::pair<std::string, usize>> found_outputs{};
      found_outputs.reserve(num_outputs);

      for (i32 i = 0; i < num_outputs; ++i)
      {
        GLint results[3];
        glGetProgramResourceiv(program, GL_PROGRAM_OUTPUT, i, properties.size(), properties.data(), properties.size(), nullptr, results);
        std::vector<char> name_buffer(results[0]);
        glGetProgramResourceName(program, GL_PROGRAM_OUTPUT, i, name_buffer.size(), nullptr, name_buffer.data());
        std::string name(name_buffer.begin(), name_buffer.end() - 1);
        GLenum type = results[1];
        found_outputs.push_back({name, type});
      }

      bool output_not_found = false, output_wrong_type = false;
      std::string_view output_not_found_name;
      std::string output_wrong_type_message;

      /*
      check if the c++ code requires outputs that are not present in the compiled shader
      */
      for_constexpr<0, std::tuple_size_v<decltype(shader_outputs)>, 1>(
          [&](auto ite)
          {
            if (output_not_found || output_wrong_type)
            {
              return;
            }

            bool current_found = false, current_right_type = false;
            GLenum _wrong_type               = 0;
            constexpr static auto shader_out = std::get<ite>(shader_outputs);

            for (const auto &_out_name : found_outputs)
            {
              if (shader_out.name == _out_name.first)
              {
                current_found = true;
                if (glsl_type_translation<typename decltype(shader_out)::type>::gl_type == _out_name.second)
                {
                  current_right_type = true;
                }
                else
                {
                  _wrong_type = _out_name.second;
                }
              }
            }
            if (!current_found)
            {
              output_not_found_name = shader_out.name;
              output_not_found      = true;
            }

            if (!current_right_type)
            {
              output_wrong_type_message =
                  "output expteced type: " + std::string(glsl_type_name_map<typename decltype(shader_out)::type>::value) +
                  " got: " + std::string(glenum_type_to_strview(_wrong_type));
              output_wrong_type = true;
            }
          });

      /*
       check if there are outputs declared and used in the shader file that the c++ code doesnt know about
       */
      for (const auto &_out_name : found_outputs)
      {
        bool current_needed = false;
        for_constexpr<0, std::tuple_size_v<decltype(shader_outputs)>, 1>(
            [&](auto ite)
            {
              constexpr static auto shader_out = std::get<ite>(shader_outputs);
              if (shader_out.name == _out_name.first)
              {
                current_needed = true;
              }
            });

        if (!current_needed)
        {
          LOG(INFO) << "shader output: " << _out_name.first << " is declared in shader file but not in c++ class";
          assert(false);
        }
      }

      if (output_not_found)
      {
        LOG(INFO) << "could not find required output in shader file: " << output_not_found_name << " in shader: " << fragment_name;
        assert(false);
      }

      if (output_wrong_type)
      {
        LOG(INFO) << output_wrong_type_message << " in shader: " << fragment_name;
        assert(false);
      }
    }

    {
      std::vector<std::pair<std::string, GLuint>> uniforms;
      GLint numUniforms = 0;
      glGetProgramiv(internal_shader.id, GL_ACTIVE_UNIFORMS, &numUniforms);
      for (GLint i = 0; i < numUniforms; ++i)
      {
        char name[256];
        GLsizei length = 0;
        GLint size     = 0;
        GLenum type    = 0;
        glGetActiveUniform(internal_shader.id, i, sizeof(name), &length, &size, &type, name);
        uniforms.emplace_back(std::string(name, length), type);
      }

      bool uniform_not_found = false, uniform_wrong_type = false;
      std::string_view uniform_not_found_name;
      std::string uniform_wrong_type_message;
      /*
      check if the c++ code requires uniforms that are not present in the compiled shader
      */
      for_constexpr<0, std::tuple_size_v<decltype(shader_uniforms)>, 1>(
          [&](auto ite)
          {
            if (uniform_not_found || uniform_wrong_type)
            {
              return;
            }

            bool current_found = false, current_right_type = false;
            GLenum _wrong_type                   = 0;
            constexpr static auto shader_uniform = std::get<ite>(shader_uniforms);

            for (const auto &_found_uniform : uniforms)
            {
              if (shader_uniform.name == _found_uniform.first)
              {
                current_found = true;
                if (glsl_type_translation<typename decltype(shader_uniform)::type>::gl_type == _found_uniform.second)
                {
                  current_right_type = true;
                }
                else
                {
                  _wrong_type = _found_uniform.second;
                }
              }
            }
            if (!current_found)
            {
              uniform_not_found_name = shader_uniform.name;
              uniform_not_found      = true;
            }

            if (!current_right_type)
            {
              uniform_wrong_type_message =
                  "uniform expteced type: " + std::string(glsl_type_name_map<typename decltype(shader_uniform)::type>::value) +
                  " got: " + std::string(glenum_type_to_strview(_wrong_type));
              uniform_wrong_type = true;
            }
          });

      /*
      check if there are uniforms declared and used in the shader file that the c++ code doesnt know about
      */
      for (const auto &_found_uniform : uniforms)
      {
        bool current_needed = false;
        for_constexpr<0, std::tuple_size_v<decltype(shader_uniforms)>, 1>(
            [&](auto ite)
            {
              constexpr static auto shader_uniform = std::get<ite>(shader_uniforms);
              if (shader_uniform.name == _found_uniform.first)
              {
                current_needed = true;
              }
            });

        if (!current_needed)
        {
          LOG(INFO) << "shader uniform: " << _found_uniform.first << " is declared in shader file but not in c++ class";
          assert(false);
        }
      }

      if (uniform_not_found)
      {
        LOG(INFO) << "could not find required output in shader file: " << uniform_not_found_name << " in shader: " << fragment_name;
        assert(false);
      }

      if (uniform_wrong_type)
      {
        LOG(INFO) << uniform_wrong_type_message << " in shader: " << fragment_name;
        assert(false);
      }
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
// 500 lines yay!