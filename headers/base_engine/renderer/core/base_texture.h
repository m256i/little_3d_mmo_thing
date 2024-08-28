#pragma once

#include <cassert>
#include <common.h>
#include <glad/glad.h>

namespace renderer::core
{

struct base_texture2d
{
  enum class texture_format
  {
    rgba,
    rgb,
    r,

    rgba16f,
    rgb16f,
    r16f,

    rgba_compressed,
    rgb_compressed,
    r_compressed,

    depth,
    depth_stencil
  };

  u0
  initialize(u32 _sx, u32 _sy, texture_format _format, bool is_reinit = false)
  {
    assert(handle == 0);

    size_x = _sx;
    size_y = _sy;

    if (!is_reinit)
    {
      format = _format;
    }

    u32 gl_internal_format = 0, gl_base_format = 0, gl_type = 0;

    switch (_format)
    {
    case texture_format::rgba:
    {
      gl_internal_format = GL_RGBA;
      gl_base_format     = GL_RGBA;
      gl_type            = GL_UNSIGNED_BYTE;
      break;
    }
    case texture_format::rgb:
    {
      gl_internal_format = GL_RGB;
      gl_base_format     = GL_RGB;
      gl_type            = GL_UNSIGNED_BYTE;
      break;
    }
    case texture_format::r:
    {
      gl_internal_format = GL_RED;
      gl_base_format     = GL_RED;
      gl_type            = GL_UNSIGNED_BYTE;
      break;
    }
    case texture_format::rgba16f:
    {
      gl_internal_format = GL_RGBA16F;
      gl_base_format     = GL_RGBA;
      gl_type            = GL_HALF_FLOAT;
      break;
    }
    case texture_format::rgb16f:
    {
      gl_internal_format = GL_RGB16F;
      gl_base_format     = GL_RGB;
      gl_type            = GL_HALF_FLOAT;
      break;
    }
    case texture_format::r16f:
    {
      gl_internal_format = GL_R16F;
      gl_base_format     = GL_RED;
      gl_type            = GL_HALF_FLOAT;
      break;
    }
    case texture_format::rgba_compressed:
    {
      /*
      TODO: to be implemented
      */
      gl_internal_format = GL_RGBA;
      gl_base_format     = GL_RGBA;
      gl_type            = GL_UNSIGNED_BYTE;
      break;
    }
    case texture_format::rgb_compressed:
    {
      /*
      TODO: to be implemented
      */
      gl_internal_format = GL_RGB;
      gl_base_format     = GL_RGB;
      gl_type            = GL_UNSIGNED_BYTE;
      break;
    }
    case texture_format::r_compressed:
    {
      gl_internal_format = GL_RED;
      gl_base_format     = GL_RED;
      gl_type            = GL_UNSIGNED_BYTE;
      break;
    }
    case texture_format::depth:
    {
      gl_internal_format = GL_DEPTH_COMPONENT;
      gl_base_format     = GL_DEPTH_COMPONENT;
      gl_type            = GL_FLOAT;
      break;
    }
    case texture_format::depth_stencil:
    {
      gl_internal_format = GL_DEPTH24_STENCIL8;
      gl_base_format     = GL_DEPTH_STENCIL;
      gl_type            = GL_UNSIGNED_INT_24_8;
      break;
    }
    }

    /* set up texture */
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, _sx, _sy, 0, gl_base_format, gl_type, NULL);

    if (_format != texture_format::depth && _format != texture_format::depth_stencil)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  u0
  destroy()
  {
    glDeleteTextures(1, &handle);
  }

  u0
  resize(u32 _nsx, u32 _nsy)
  {
    if (_nsx == size_x && _nsy == size_y) [[unlikely]]
    {
      return;
    }

    assert(handle != 0);

    size_x = _nsx;
    size_y = _nsy;

    destroy();
    initialize(_nsx, _nsy, format, true);
  }

  texture_format format{};
  u32 size_x, size_y;
  u32 handle{};
};

} // namespace renderer::core