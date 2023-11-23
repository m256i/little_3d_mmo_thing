#include <base_engine/renderer/shader.h>

#include <glm/glm.hpp>

#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>

#include <logging/easylogging++.h>
#include "../../../common.h"

#include <glad/glad.h>

bool
basic_shader_t::load_from_path(std::string_view _vertex_path, std::string_view _frag_path, const char *_geo_path)
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
//