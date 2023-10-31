#pragma once

#include <glm/glm.hpp>

#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../../logging/easylogging++.h"
#include "../../../common.h"

#include <glad/glad.h>

struct basic_shader_t
{
  u32 id{};
  std::string_view debug_name{};

  basic_shader_t(std::string_view _debug_name) : debug_name(_debug_name){};
  basic_shader_t(std::string_view _debug_name, std::string_view _vertex_path, std::string_view _frag_path, const char *_geo_path = nullptr) : debug_name(_debug_name)
  {
    if (!load_from_path(_vertex_path, _frag_path, _geo_path))
    {
      LOG(INFO) << "[shader] : error loading shader" << debug_name;
    }
    LOG(INFO) << "[shader] : " << debug_name << "compiled succesfully";
  }

  bool
  load_from_path(std::string_view _vertex_path, std::string_view _frag_path, const char *_geo_path = nullptr)
  {
    bool status = true;
    std::string vertex_code, frag_code, geo_code;
    std::ifstream vert_shader_file, frag_shader_file, geo_shader_file;

    vert_shader_file.open(_vertex_path.data());
    frag_shader_file.open(_frag_path.data());

    std::stringstream vert_shader_stream, frag_shader_stream;

    vert_shader_stream << vert_shader_file.rdbuf();
    frag_shader_stream << frag_shader_file.rdbuf();
    vert_shader_file.close();
    frag_shader_file.close();
    vertex_code = vert_shader_stream.str();
    frag_code   = frag_shader_stream.str();

    if (vertex_code.empty())
    {
      LOG(INFO) << "[shaders] : failed to load vertex shader from file: " << _vertex_path;
      status = false;
    }
    if (frag_code.empty())
    {
      LOG(INFO) << "[shaders] : failed to load fragment shader from file: " << _frag_path;
      status = false;
    }

    if (_geo_path != nullptr)
    {
      geo_shader_file.open(_geo_path);
      std::stringstream geo_shader_stream;
      geo_shader_stream << geo_shader_file.rdbuf();
      geo_shader_file.close();
      geo_code = geo_shader_stream.str();

      if (vertex_code.empty())
      {
        LOG(INFO) << "[shaders] : failed to load geometry shader from file: " << _vertex_path;
        status = false;
      }
    }

    if (!status)
    {
      return status;
    }

    const char *vert_shader_code = vertex_code.data();
    const char *frag_shader_code = frag_code.data();

    u32 vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex, 1, &vert_shader_code, NULL);
    glCompileShader(vertex);

    if (!check_shadercomp_errors(vertex, "VERTEX"))
    {
      return false;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment, 1, &frag_shader_code, NULL);
    glCompileShader(fragment);

    if (!check_shadercomp_errors(fragment, "FRAGMENT"))
    {
      return false;
    }

    u32 geometry;
    if (_geo_path != nullptr)
    {
      const char *geo_shader_code = geo_code.data();
      geometry                    = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geometry, 1, &geo_shader_code, NULL);
      glCompileShader(geometry);
      if (!check_shadercomp_errors(geometry, "GEOMETRY"))
      {
        return false;
      }
    }

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);

    if (_geo_path != nullptr)
    {
      glAttachShader(id, geometry);
    }

    glLinkProgram(id);

    if (!check_shadercomp_errors(id, "PROGRAM"))
    {
      return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (_geo_path != nullptr)
    {
      glDeleteShader(geometry);
    }

    LOG(INFO) << "[shader] : shader " << debug_name << " loaded successfully";
    return true;
  }

  u0
  use() const
  {
    glUseProgram(id);
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
        LOG(INFO) << "[shader] : error compiling shader: " << _type << "\n" << out_log << "\n -- --------------------------------------------------- -- ";
        return false;
      }
    }
    else
    {
      glGetProgramiv(_shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(_shader, 1024, NULL, out_log);
        LOG(INFO) << "[shader] : error compiling shader: " << _type << "\n" << out_log << "\n -- --------------------------------------------------- -- ";
        return false;
      }
    }
    return true;
  }
};
