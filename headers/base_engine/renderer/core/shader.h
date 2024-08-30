#pragma once

#include "gl_type_translations.h"
#include <common.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <filesystem>

#include <base_engine/utils/fnv1a.h>

namespace renderer::core
{

template <ct_string TName, typename TType>
struct shader_uniform_handle
{
  constexpr static std::string_view name = TName.to_view();
  using type                             = TType;
  type object{};
};

/*
TODO: use custom arena allocator for optional here for less cache misses
*/
template <typename... TUniforms>
struct shader_uniform_pack
{
  template <ct_string TName, typename TType>
  std::optional<std::reference_wrapper<TType>>
  get()
  {
    TType *ptr = nullptr;
    for_constexpr<0, sizeof...(TUniforms), 1>(
        [&](auto ite)
        {
          auto &curr = std::get<ite>(uniforms);
          if (curr.name == TName.to_view() && std::is_same_v<TType, typename std::remove_cvref_t<decltype(curr)>::type>)
          {
            ptr = std::addressof(curr.object);
          }
        });
    if (ptr != nullptr)
    {
      return std::make_optional(std::ref(*ptr));
    }
    else
    {
      return std::nullopt;
    }
  }

  template <ct_string TName, typename TType>
  std::optional<std::reference_wrapper<const TType>>
  get() const
  {
    const TType *ptr = nullptr;
    for_constexpr<0, sizeof...(TUniforms), 1>(
        [&](auto ite)
        {
          auto &curr = std::get<ite>(uniforms);
          if (curr.name == TName.to_view() && std::is_same_v<TType, typename std::remove_cvref_t<decltype(curr)>::type>)
          {
            ptr = std::addressof(curr.object);
          }
        });
    if (ptr != nullptr)
    {
      return std::make_optional(std::ref(*ptr));
    }
    else
    {
      return std::nullopt;
    }
  }

  static constexpr usize uniform_count = sizeof...(TUniforms);

  std::tuple<TUniforms...> uniforms{};
};

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
  set(std::string_view _name, int _value) const
  {
    glUniform1i(glGetUniformLocation(id, _name.data()), _value);
  }

  u0
  set(std::string_view _name, float _value) const
  {
    glUniform1f(glGetUniformLocation(id, _name.data()), _value);
  }

  u0
  set(std::string_view _name, const glm::vec2 &_value) const
  {
    glUniform2fv(glGetUniformLocation(id, _name.data()), 1, &_value[0]);
  }

  u0
  set(std::string_view _name, const glm::vec3 &_value) const
  {
    glUniform3fv(glGetUniformLocation(id, _name.data()), 1, &_value[0]);
  }

  u0
  set(std::string_view _name, const glm::vec4 &_value) const
  {
    glUniform4fv(glGetUniformLocation(id, _name.data()), 1, &_value[0]);
  }

  u0
  set(std::string_view _name, const glm::mat2 &_mat) const
  {
    glUniformMatrix2fv(glGetUniformLocation(id, _name.data()), 1, GL_FALSE, &_mat[0][0]);
  }

  u0
  set(std::string_view _name, const glm::mat3 &_mat) const
  {
    glUniformMatrix3fv(glGetUniformLocation(id, _name.data()), 1, GL_FALSE, &_mat[0][0]);
  }

  u0
  set(std::string_view _name, const glm::mat4 &_mat) const
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

  template <typename T>
  struct to_shader_uniform_handle;

  template <ct_string TName, typename TType>
  struct to_shader_uniform_handle<shader_uniform<TName, TType>>
  {
    using type = shader_uniform_handle<TName, TType>;
  };

  template <typename Tuple, std::size_t... I>
  static constexpr auto
  make_shader_uniform_pack_impl(Tuple &&tuple, std::index_sequence<I...>)
  {
    return shader_uniform_pack<typename to_shader_uniform_handle<std::tuple_element_t<I, Tuple>>::type...>{};
  }

  template <typename Tuple>
  static constexpr auto
  make_shader_uniform_pack(Tuple &&tuple)
  {
    constexpr std::size_t N = std::tuple_size_v<std::decay_t<Tuple>>;
    return make_shader_uniform_pack_impl(std::forward<Tuple>(tuple), std::make_index_sequence<N>{});
  }

public:
  static constexpr auto shader_inputs  = filter_types_t<is_shader_input, TShaderInOutNodes...>{};
  static constexpr auto shader_outputs = filter_types_t<is_shader_output, TShaderInOutNodes...>{};
  static constexpr auto shader_uniforms =
      std::tuple_cat(filter_types_t<is_shader_uniform, TShaderInOutNodes...>{}, filter_types_t<is_texture_input, TShaderInOutNodes...>{});

  static constexpr auto texture_inputs = filter_types_t<is_texture_input, TShaderInOutNodes...>{};

  detail::internal_shader internal_shader{TShaderName.to_view()};

  decltype(make_shader_uniform_pack(filter_types_t<is_shader_uniform, TShaderInOutNodes...>{})) uniforms =
      make_shader_uniform_pack(filter_types_t<is_shader_uniform, TShaderInOutNodes...>{});

  bool has_vertex = false, has_fragment = false, has_tesc = false, has_tese = false;
  bool initialized = false;

  template <ct_string TName, typename TType>
  decltype(auto)
  get_uniform()
  {
    auto value = uniforms.template get<TName, TType>();
    if (value.has_value())
    {
      return (value->get());
    }
    else
    {
      LOG(INFO) << "tried to acces uniform" << TName.to_view() << "which doesn't exist!";
      assert(false);
      std::unreachable();
    }
  }

  /*
    @TODO: code cleanup some day later. this works but its really ugly
  */
  bool
  initialize()
  {
    if (initialized)
    {
      return false;
    }

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

    static const auto check_lambda =
        [&](const std::vector<std::pair<std::string, u32>> &_name_type_list, auto _required_tuple, std::string_view object_type)
    {
      bool not_found = false, wrong_type = false;
      std::string_view not_found_name;
      std::string wrong_type_message;

      /*
      check if the c++ code requires attributes that are not present in the compiled shader
      */
      for_constexpr<0, std::tuple_size_v<decltype(_required_tuple)>, 1>(
          [&](auto ite)
          {
            if (not_found || wrong_type)
            {
              return;
            }

            bool current_found = false, current_right_type = false;
            GLenum _wrong_type               = 0;
            constexpr static auto shader_ipt = std::get<ite>(_required_tuple);

            for (const auto &_ipt_name : _name_type_list)
            {
              if (shader_ipt.name == _ipt_name.first)
              {
                current_found = true;
                if constexpr (std::is_same_v<typename decltype(shader_ipt)::type, detail::impl_unique_shader_tag_type>)
                {
                  if (_ipt_name.second == GL_SAMPLER_2D)
                  {
                    current_right_type = true;
                  }
                  else
                  {
                    _wrong_type = _ipt_name.second;
                  }
                }
                else
                {
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
            }

            if (!current_found)
            {
              not_found_name = shader_ipt.name;
              not_found      = true;
            }

            if (!current_right_type)
            {
              if constexpr (std::is_same_v<typename decltype(shader_ipt)::type, detail::impl_unique_shader_tag_type>)
              {
                wrong_type_message =
                    std::string(object_type.data()) + " expteced type sampler2d, got: " + std::string(glenum_type_to_strview(_wrong_type));
              }
              else
              {
                wrong_type_message = std::string(object_type.data()) +
                                     " expteced type: " + std::string(glsl_type_name_map<typename decltype(shader_ipt)::type>::value) +
                                     " got: " + std::string(glenum_type_to_strview(_wrong_type));
              }
              wrong_type = true;
            }
          });

      /*
       check if there are attributes declared and used in the shader file that the c++ code doesnt know about
       */
      for (const auto &_ipt_name : _name_type_list)
      {
        bool current_needed = false;
        for_constexpr<0, std::tuple_size_v<decltype(_required_tuple)>, 1>(
            [&](auto ite)
            {
              constexpr static auto shader_ipt = std::get<ite>(_required_tuple);
              if (shader_ipt.name == _ipt_name.first)
              {
                current_needed = true;
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
        LOG(INFO) << "could not find required " << object_type << " in shader file: " << not_found_name << " in shader: " << vertex_name;
        assert(false);
      }
      if (wrong_type)
      {
        LOG(INFO) << wrong_type_message << " in shader: " << vertex_name;
        assert(false);
      }
    };

    static const auto input_generator = [&]()
    {
      char name[129];
      i32 name_length = 0, io_count{};
      glGetProgramiv(internal_shader.id, GL_ACTIVE_ATTRIBUTES, &io_count);

      std::vector<std::pair<std::string, u32>> found_inputs{};
      found_inputs.reserve(io_count);

      for (i32 i = 0; i < io_count; i++)
      {
        i32 size;
        GLenum type;
        glGetActiveAttrib(internal_shader.id, i, sizeof(name), &name_length, &size, &type, name);
        found_inputs.push_back({std::string(name), type});
      }
      return found_inputs;
    };

    static const auto output_generator = [&]()
    {
      const GLuint program = internal_shader.id;
      GLint num_outputs;

      glGetProgramInterfaceiv(program, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &num_outputs);
      std::array properties{(GLenum)GL_NAME_LENGTH, (GLenum)GL_TYPE};

      std::vector<std::pair<std::string, u32>> found_outputs{};
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
      return found_outputs;
    };

    static const auto uniform_generator = [&]()
    {
      std::vector<std::pair<std::string, u32>> uniforms;
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
      return uniforms;
    };

    check_lambda(input_generator(), shader_inputs, "input");
    check_lambda(output_generator(), shader_outputs, "output");
    check_lambda(uniform_generator(), shader_uniforms, "uniform");

    initialized = true;
    return true;
  }

  u0
  bind(const std::array<u32, std::tuple_size_v<decltype(texture_inputs)>> &texure_handles)
  {
    assert(initialized);
    /*
    update uniform values
    */
    for_constexpr<0, uniforms.uniform_count, 1>(
        [&](auto ite)
        {
          auto &uniform = std::get<ite>(uniforms.uniforms);
          internal_shader.set(uniform.name, uniform.object);
        });

    /*
    bind the textures
    */
    for_constexpr<0, std::tuple_size_v<decltype(texture_inputs)>, 1>(
        [&](auto ite)
        {
          auto &texture = std::get<ite>(texture_inputs);
          internal_shader.set(texture.name, (i32)texure_handles[ite]);
        });

    internal_shader.use();
  }

  /*
  pass with no textures
  */
  u0
  bind()
  {
    assert(initialized);
    /*
    update uniform values
    */
    for_constexpr<0, uniforms.uniform_count, 1>(
        [&](auto ite)
        {
          auto &uniform = std::get<ite>(uniforms.uniforms);
          internal_shader.set(uniform.name, uniform.object);
        });

    internal_shader.use();
  }

  u0
  destroy()
  {
    assert(initialized);
    glDeleteShader(internal_shader.id);
    initialized = false;
  }
};

} // namespace renderer::core
// 500 lines yay!