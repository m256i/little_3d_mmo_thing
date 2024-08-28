#pragma once

#include <common.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace renderer::core
{

/*
we support sending a fixed list of types to the GPU:
float
int
uint
vec2f
vec3f
vec4f
mat3f
mat4f
*/

inline constexpr std::string_view
glenum_type_to_strview(GLenum _enum)
{
  switch (_enum)
  {
  case GL_FLOAT:
  {
    return {"float"};
  }
  case GL_INT:
  {
    return {"int"};
  }
  case GL_UNSIGNED_INT:
  {
    return {"uint"};
  }
  case GL_FLOAT_VEC2:
  {
    return {"vec2"};
  }
  case GL_FLOAT_VEC3:
  {
    return {"vec3"};
  }
  case GL_FLOAT_VEC4:
  {
    return {"vec4"};
  }
  case GL_FLOAT_MAT3:
  {
    return {"mat3"};
  }
  case GL_FLOAT_MAT4:
  {
    return {"mat4"};
  }
  case GL_SAMPLER_2D:
  {
    return {"sampler2d"};
  }
  default:
  {
    break;
  }
  }
  return {""};
}

using allowed_types = std::tuple<f32, i32, u32, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

template <typename TType>
struct glsl_type_name_map;

template <typename TType>
struct glsl_type_translation;

template <>
struct glsl_type_name_map<f32>
{
  constexpr static std::string_view value{"float"};
};

template <>
struct glsl_type_translation<f32>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(f32) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT;
};

template <>
struct glsl_type_name_map<i32>
{
  constexpr static std::string_view value{"int"};
};

template <>
struct glsl_type_translation<i32>
{
  constexpr static usize type_size  = sizeof(i32);
  constexpr static usize type_count = sizeof(i32) / sizeof(i32);
  constexpr static GLenum gl_type   = GL_INT;
};

template <>
struct glsl_type_name_map<u32>
{
  constexpr static std::string_view value{"uint"};
};

template <>
struct glsl_type_translation<u32>
{
  constexpr static usize type_size  = sizeof(u32);
  constexpr static usize type_count = sizeof(u32) / sizeof(u32);
  constexpr static GLenum gl_type   = GL_UNSIGNED_INT;
};

template <>
struct glsl_type_name_map<glm::vec2>
{
  constexpr static std::string_view value{"vec2"};
};

template <>
struct glsl_type_translation<glm::vec2>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::vec2) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT_VEC2;
};

template <>
struct glsl_type_name_map<glm::vec3>
{
  constexpr static std::string_view value{"vec3"};
};

template <>
struct glsl_type_translation<glm::vec3>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::vec3) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT_VEC3;
};

template <>
struct glsl_type_name_map<glm::vec4>
{
  constexpr static std::string_view value{"vec4"};
};

template <>
struct glsl_type_translation<glm::vec4>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::vec4) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT_VEC4;
};

template <>
struct glsl_type_name_map<glm::mat3>
{
  constexpr static std::string_view value{"mat3"};
};

template <>
struct glsl_type_translation<glm::mat3>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::mat3) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT_MAT3;
};

template <>
struct glsl_type_name_map<glm::mat4>
{
  constexpr static std::string_view value{"mat4"};
};

template <>
struct glsl_type_translation<glm::mat4>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::mat4) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT_MAT4;
};
} // namespace renderer::core