#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string_view>

#include <glm/gtc/matrix_transform.hpp>

struct simple_shader_t
{
  std::size_t index;

public:
  simple_shader_t(const char* vert_path, const char* fragment_path);

public:
  void use() const;

  void set_bool(const std::string& name, bool value) const;
  void set_int(const std::string& name, int value) const;
  void set_float(const std::string& name, float value) const;
  void set_mat4(const std::string& name, glm::mat4 value) const;

  void
  set_vec3(const std::string& name, const glm::vec3& value) const
  {
    glUniform3fv(glGetUniformLocation(index, name.c_str()), 1, &value[0]);
  }
  void
  set_vec3(const std::string& name, float x, float y, float z) const
  {
    glUniform3f(glGetUniformLocation(index, name.c_str()), x, y, z);
  }

  void
  set_vec2(const std::string& name, float x, float y) const
  {
    glUniform2f(glGetUniformLocation(index, name.c_str()), x, y);
  }

  void
  set_vec2(const std::string& name, const glm::vec2& value) const
  {
    glUniform2fv(glGetUniformLocation(index, name.c_str()), 1, &value[0]);
  }
};