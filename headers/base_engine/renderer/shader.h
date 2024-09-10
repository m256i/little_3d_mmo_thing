#pragma once

#include <glm/glm.hpp>
#include <string_view>
#include <logging/easylogging++.h>
#include <glad/glad.h>
#include <common.h>

enum class shader_attribute_type
{
  invalid = 0,
  type_f32,
  type_f64,
  type_i32,
  type_u32,
  type_vec2i,
  type_vec2f,
  type_vec3i,
  type_vec3f,
  type_vec4i,
  type_vec4f,
  type_mat2,
  type_mat3,
  type_mat4,
};

inline shader_attribute_type
shader_attrib_from_gl(GLenum type)
{
  using e = shader_attribute_type;
  // clang-format off
  switch (type)
  {
  case GL_FLOAT:        return e::type_f32;
  case GL_DOUBLE:       return e::type_f64;
  case GL_INT:          return e::type_i32;
  case GL_UNSIGNED_INT: return e::type_u32;
  case GL_INT_VEC2:     return e::type_vec2i;
  case GL_FLOAT_VEC2:   return e::type_vec2f;
  case GL_INT_VEC3:     return e::type_vec3i;
  case GL_FLOAT_VEC3:   return e::type_vec3f;
  case GL_INT_VEC4:     return e::type_vec4i;
  case GL_FLOAT_VEC4:   return e::type_vec4f;
  case GL_FLOAT_MAT2:   return e::type_mat2;
  case GL_FLOAT_MAT3:   return e::type_mat3;
  case GL_FLOAT_MAT4:   return e::type_mat4;
  }
  return e::invalid;
  // clang-format on
}

struct shader_attribute
{
  std::string name;
  shader_attribute_type type;
};

struct basic_shader_t
{

  u32 id{};
  std::string_view debug_name{};
  bool initialized{false};

  basic_shader_t(std::string_view _debug_name = "unnamed shader") : debug_name(_debug_name) {};
  basic_shader_t(std::string_view _debug_name, std::string_view _vertex_path, std::string_view _frag_path, std::string_view _tess_path = {},
                 std::string_view _tess_path_eval = {}, const char *_geo_path = nullptr)
      : debug_name(_debug_name)
  {
    if (!load_from_path(_vertex_path, _frag_path, _tess_path, _geo_path))
    {
      LOG(INFO) << "[shader] : error loading shader" << debug_name;
    }
    LOG(INFO) << "[shader] : " << debug_name << "compiled succesfully";
  }

  basic_shader_t(const basic_shader_t &_other) : id(_other.id), debug_name(_other.debug_name), initialized(_other.initialized) {}
  basic_shader_t(basic_shader_t &&_other)
      : id(std::move(_other.id)), debug_name(std::move(_other.debug_name)), initialized(std::move(_other.initialized))
  {
  }

  decltype(auto)
  operator=(const basic_shader_t &_other)
  {
    id          = _other.id;
    debug_name  = _other.debug_name;
    initialized = _other.initialized;
  }

  decltype(auto)
  operator=(basic_shader_t &&_other)
  {
    id          = std::move(_other.id);
    debug_name  = std::move(_other.debug_name);
    initialized = std::move(_other.initialized);
  }

  bool load_from_path(std::string_view _vertex_path, std::string_view _frag_path, std::string_view _tess_control_path = {},
                      std::string_view _tess_path_eval = {}, const char *_geo_path = nullptr);

  std::vector<shader_attribute>
  get_input_attributes() const
  {
    assert(initialized);
    std::vector<shader_attribute> out{};

    char name[129];
    i32 name_length = 0, io_count{};
    glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &io_count);

    out.reserve(io_count);

    for (i32 i = 0; i < io_count; i++)
    {
      i32 size;
      GLenum type;
      glGetActiveAttrib(id, i, sizeof(name), &name_length, &size, &type, name);
      out.push_back(shader_attribute{.name = std::string(name), .type = shader_attrib_from_gl(type)});
    }

    return out;
  }

  std::vector<shader_attribute>
  get_output_attributes() const
  {
    assert(initialized);
    std::vector<shader_attribute> out{};

    const GLuint program = id;
    GLint num_outputs;

    glGetProgramInterfaceiv(program, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &num_outputs);
    std::array properties{(GLenum)GL_NAME_LENGTH, (GLenum)GL_TYPE};

    out.reserve(num_outputs);

    for (i32 i = 0; i < num_outputs; ++i)
    {
      assert(initialized);
      GLint results[3];
      glGetProgramResourceiv(program, GL_PROGRAM_OUTPUT, i, properties.size(), properties.data(), properties.size(), nullptr, results);
      std::vector<char> name_buffer(results[0]);
      glGetProgramResourceName(program, GL_PROGRAM_OUTPUT, i, name_buffer.size(), nullptr, name_buffer.data());
      std::string name(name_buffer.begin(), name_buffer.end() - 1);
      GLenum type = results[1];
      out.push_back(shader_attribute{name, shader_attrib_from_gl(type)});
    }
    return out;
  }

  std::vector<shader_attribute>
  get_uniforms() const
  {
    assert(initialized);
    std::vector<shader_attribute> out;
    GLint numUniforms = 0;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numUniforms);
    for (GLint i = 0; i < numUniforms; ++i)
    {
      char name[256];
      GLsizei length = 0;
      GLint size     = 0;
      GLenum type    = 0;
      glGetActiveUniform(id, i, sizeof(name), &length, &size, &type, name);
      if (type != GL_SAMPLER_2D)
      {
        out.emplace_back(std::string(name, length), shader_attrib_from_gl(type));
      }
    }
    return out;
  }

  std::vector<std::string>
  get_input_textures() const
  {
    assert(initialized);
    std::vector<std::string> out;
    GLint numUniforms = 0;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numUniforms);
    for (GLint i = 0; i < numUniforms; ++i)
    {
      char name[256];
      GLsizei length = 0;
      GLint size     = 0;
      GLenum type    = 0;
      glGetActiveUniform(id, i, sizeof(name), &length, &size, &type, name);
      printf("found uniform %s with type %d\n", name, type);
      if (type == GL_SAMPLER_2D)
      {
        out.emplace_back(std::string(name, length));
      }
    }
    return out;
  }

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
  setInt(usize _name_hash, int _value) const
  {
    glUniform1i(hashed_uniform_handles.at(_name_hash), _value);
  }

  u0
  setFloat(std::string_view _name, float _value) const
  {
    glUniform1f(glGetUniformLocation(id, _name.data()), _value);
  }

  u0
  setFloat(usize _name_hash, float _value) const
  {
    glUniform1f(hashed_uniform_handles.at(_name_hash), _value);
  }

  u0
  setVec2(std::string_view _name, const glm::vec2 &_value) const
  {
    glUniform2fv(glGetUniformLocation(id, _name.data()), 1, &_value[0]);
  }

  u0
  setVec2(usize _name_hash, const glm::vec2 &_value) const
  {
    glUniform2fv(hashed_uniform_handles.at(_name_hash), 1, &_value[0]);
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
  setVec3(usize _name_hash, const glm::vec3 &_value) const
  {
    glUniform3fv(hashed_uniform_handles.at(_name_hash), 1, &_value[0]);
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
  setVec4(usize _name_hash, const glm::vec4 &_value) const
  {
    glUniform4fv(hashed_uniform_handles.at(_name_hash), 1, &_value[0]);
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
  setMat3(usize _name_hash, const glm::mat3 &_mat) const
  {
    glUniformMatrix3fv(hashed_uniform_handles.at(_name_hash), 1, GL_FALSE, &_mat[0][0]);
  }

  u0
  setMat4(std::string_view _name, const glm::mat4 &_mat) const
  {
    glUniformMatrix4fv(glGetUniformLocation(id, _name.data()), 1, GL_FALSE, &_mat[0][0]);
  }

  u0
  setMat4(usize _name_hash, const glm::mat4 &_mat) const
  {
    glUniformMatrix4fv(hashed_uniform_handles.at(_name_hash), 1, GL_FALSE, &_mat[0][0]);
  }

  bool
  check_shadercomp_errors(GLuint _shader, std::string_view _type)
  {
    i32 success;
    GLchar out_log[1024];
    if (_type != "PROGRAM")
    {
      glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(_shader, 1024, NULL, out_log);
        LOG(INFO) << "[shader] : error compiling shader: " << _type << "\n"
                  << out_log << "\n -- --------------------------------------------------- -- ";
        return false;
      }
    }
    else
    {
      glGetProgramiv(_shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(_shader, 1024, NULL, out_log);
        LOG(INFO) << "[shader] : error compiling shader: " << _type << "\n"
                  << out_log << "\n -- --------------------------------------------------- -- ";
        return false;
      }
    }
    return true;
  }

  std::unordered_map<usize, u32> hashed_uniform_handles{};
};
