#include <base_engine/renderer/shader.h>
#include <base_engine/renderer/core/shader.h>

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
basic_shader_t::load_from_path(std::string_view _vertex_path, std::string_view _frag_path, std::string_view _tess_path_control,
                               std::string_view _tess_path_eval, const char *_geo_path)
{
  bool status = true;
  std::string vertex_code, frag_code, geo_code, tess_control_code, tess_eval_code;
  std::ifstream vert_shader_file, frag_shader_file, geo_shader_file, tess_control_shader_file, tess_eval_shader_file;

  vert_shader_file.open(_vertex_path.data());
  frag_shader_file.open(_frag_path.data());

  std::stringstream vert_shader_stream, frag_shader_stream;

  vert_shader_stream << vert_shader_file.rdbuf();
  frag_shader_stream << frag_shader_file.rdbuf();

  vert_shader_file.close();
  frag_shader_file.close();

  vertex_code = vert_shader_stream.str();
  frag_code   = frag_shader_stream.str();

  if (vertex_code.empty() && !_vertex_path.empty())
  {
    LOG(INFO) << "[shaders] : failed to load vertex shader from file: " << _vertex_path;
    status = false;
  }
  if (frag_code.empty() && !_frag_path.empty())
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
      LOG(INFO) << "[shaders] : failed to load geometry shader from file: " << _geo_path;
      status = false;
    }
  }

  if (!_tess_path_control.empty())
  {
    tess_control_shader_file.open(_tess_path_control.data());
    std::stringstream tess_shader_stream;
    tess_shader_stream << tess_control_shader_file.rdbuf();
    tess_control_shader_file.close();
    tess_control_code = tess_shader_stream.str();

    if (tess_control_code.empty())
    {
      LOG(INFO) << "[shaders] : failed to load tess_control shader from file: " << _tess_path_control;
      status = false;
    }
  }

  if (!_tess_path_eval.empty())
  {
    tess_eval_shader_file.open(_tess_path_eval.data());
    std::stringstream tess_shader_stream_eval;
    tess_shader_stream_eval << tess_eval_shader_file.rdbuf();
    tess_eval_shader_file.close();
    tess_eval_code = tess_shader_stream_eval.str();

    if (tess_eval_code.empty())
    {
      LOG(INFO) << "[shaders] : failed to load tess_eval shader from file: " << _tess_path_eval;
      status = false;
    }
  }

  if (!status)
  {
    return status;
  }

  const char *vert_shader_code = vertex_code.empty() ? nullptr : vertex_code.data();
  const char *frag_shader_code = frag_code.empty() ? nullptr : frag_code.data();

  u32 vertex, fragment;
  if (vert_shader_code)
  {
    vertex = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex, 1, &vert_shader_code, NULL);
    glCompileShader(vertex);

    if (!check_shadercomp_errors(vertex, "VERTEX"))
    {
      return false;
    }
  }

  if (frag_shader_code)
  {
    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment, 1, &frag_shader_code, NULL);
    glCompileShader(fragment);

    if (!check_shadercomp_errors(fragment, "FRAGMENT"))
    {
      return false;
    }
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

  u32 tess_control;
  if (!_tess_path_control.empty())
  {
    const char *tess_control_shader_code = tess_control_code.data();
    tess_control                         = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tess_control, 1, &tess_control_shader_code, NULL);
    glCompileShader(tess_control);
    if (!check_shadercomp_errors(tess_control, "TESS_CONTROL"))
    {
      return false;
    }
  }

  u32 tess_eval;
  if (!_tess_path_eval.empty())
  {
    const char *tess_eval_shader_code = tess_eval_code.data();
    tess_eval                         = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tess_eval, 1, &tess_eval_shader_code, NULL);
    glCompileShader(tess_eval);
    if (!check_shadercomp_errors(tess_eval, "TESS_EVALUATION_SHADER"))
    {
      return false;
    }
  }

  id = glCreateProgram();

  if (vert_shader_code)
  {
    glAttachShader(id, vertex);
  }
  if (frag_shader_code)
  {
    glAttachShader(id, fragment);
  }

  if (_geo_path != nullptr)
  {
    glAttachShader(id, geometry);
  }

  if (!_tess_path_control.empty())
  {
    glAttachShader(id, tess_control);
  }

  if (!_tess_path_eval.empty())
  {
    glAttachShader(id, tess_eval);
  }

  glLinkProgram(id);

  if (!check_shadercomp_errors(id, "PROGRAM"))
  {
    return false;
  }

  if (vertex)
  {
    glDeleteShader(vertex);
  }
  if (fragment)
  {
    glDeleteShader(fragment);
  }

  if (_geo_path != nullptr)
  {
    glDeleteShader(geometry);
  }

  if (!_tess_path_control.empty())
  {
    glDeleteShader(tess_control);
  }

  if (!_tess_path_eval.empty())
  {
    glDeleteShader(tess_eval);
  }

  LOG(INFO) << "[shader] : shader " << debug_name << " loaded successfully";
  return true;
}

bool
renderer::core::detail::internal_shader::load_from_path(std::string_view _vertex_path, std::string_view _frag_path,
                                                        std::string_view _tess_path_control, std::string_view _tess_path_eval,
                                                        const char *_geo_path)
{
  bool status = true;
  std::string vertex_code, frag_code, geo_code, tess_control_code, tess_eval_code;
  std::ifstream vert_shader_file, frag_shader_file, geo_shader_file, tess_control_shader_file, tess_eval_shader_file;

  vert_shader_file.open(_vertex_path.data());
  frag_shader_file.open(_frag_path.data());

  std::stringstream vert_shader_stream, frag_shader_stream;

  vert_shader_stream << vert_shader_file.rdbuf();
  frag_shader_stream << frag_shader_file.rdbuf();

  vert_shader_file.close();
  frag_shader_file.close();

  vertex_code = vert_shader_stream.str();
  frag_code   = frag_shader_stream.str();

  if (vertex_code.empty() && !_vertex_path.empty())
  {
    LOG(INFO) << "[shaders] : failed to load vertex shader from file: " << _vertex_path;
    status = false;
  }
  if (frag_code.empty() && !_frag_path.empty())
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
      LOG(INFO) << "[shaders] : failed to load geometry shader from file: " << _geo_path;
      status = false;
    }
  }

  if (!_tess_path_control.empty())
  {
    tess_control_shader_file.open(_tess_path_control.data());
    std::stringstream tess_shader_stream;
    tess_shader_stream << tess_control_shader_file.rdbuf();
    tess_control_shader_file.close();
    tess_control_code = tess_shader_stream.str();

    if (tess_control_code.empty())
    {
      LOG(INFO) << "[shaders] : failed to load tess_control shader from file: " << _tess_path_control;
      status = false;
    }
  }

  if (!_tess_path_eval.empty())
  {
    tess_eval_shader_file.open(_tess_path_eval.data());
    std::stringstream tess_shader_stream_eval;
    tess_shader_stream_eval << tess_eval_shader_file.rdbuf();
    tess_eval_shader_file.close();
    tess_eval_code = tess_shader_stream_eval.str();

    if (tess_eval_code.empty())
    {
      LOG(INFO) << "[shaders] : failed to load tess_eval shader from file: " << _tess_path_eval;
      status = false;
    }
  }

  if (!status)
  {
    return status;
  }

  const char *vert_shader_code = vertex_code.empty() ? nullptr : vertex_code.data();
  const char *frag_shader_code = frag_code.empty() ? nullptr : frag_code.data();

  u32 vertex, fragment;
  if (vert_shader_code)
  {
    vertex = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex, 1, &vert_shader_code, NULL);
    glCompileShader(vertex);

    if (!check_shadercomp_errors(vertex, "VERTEX"))
    {
      return false;
    }
  }

  if (frag_shader_code)
  {
    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment, 1, &frag_shader_code, NULL);
    glCompileShader(fragment);

    if (!check_shadercomp_errors(fragment, "FRAGMENT"))
    {
      return false;
    }
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

  u32 tess_control;
  if (!_tess_path_control.empty())
  {
    const char *tess_control_shader_code = tess_control_code.data();
    tess_control                         = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tess_control, 1, &tess_control_shader_code, NULL);
    glCompileShader(tess_control);
    if (!check_shadercomp_errors(tess_control, "TESS_CONTROL"))
    {
      return false;
    }
  }

  u32 tess_eval;
  if (!_tess_path_eval.empty())
  {
    const char *tess_eval_shader_code = tess_eval_code.data();
    tess_eval                         = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tess_eval, 1, &tess_eval_shader_code, NULL);
    glCompileShader(tess_eval);
    if (!check_shadercomp_errors(tess_eval, "TESS_EVALUATION_SHADER"))
    {
      return false;
    }
  }

  id = glCreateProgram();

  if (vert_shader_code)
  {
    glAttachShader(id, vertex);
  }
  if (frag_shader_code)
  {
    glAttachShader(id, fragment);
  }

  if (_geo_path != nullptr)
  {
    glAttachShader(id, geometry);
  }

  if (!_tess_path_control.empty())
  {
    glAttachShader(id, tess_control);
  }

  if (!_tess_path_eval.empty())
  {
    glAttachShader(id, tess_eval);
  }

  glLinkProgram(id);

  if (!check_shadercomp_errors(id, "PROGRAM"))
  {
    return false;
  }

  if (vertex)
  {
    glDeleteShader(vertex);
  }
  if (fragment)
  {
    glDeleteShader(fragment);
  }

  if (_geo_path != nullptr)
  {
    glDeleteShader(geometry);
  }

  if (!_tess_path_control.empty())
  {
    glDeleteShader(tess_control);
  }

  if (!_tess_path_eval.empty())
  {
    glDeleteShader(tess_eval);
  }

  LOG(INFO) << "[shader] : shader " << debug_name << " compiled successfully";
  return true;
}
//