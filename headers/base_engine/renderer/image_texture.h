#pragma once

#include "base_engine/renderer/shader.h"
#include <include/stb_image.h>
#include <GL/gl.h>
#include <glad/glad.h>

#include <common.h>

namespace renderer
{
struct image_tex
{
  u0
  load(const char* _path)
  {
    int channels;
    unsigned char* data = stbi_load(_path, &size_x, &size_y, &channels, STBI_rgb);

    if (data)
    {
      glGenTextures(1, &tex_handle);
      glBindTexture(GL_TEXTURE_2D, tex_handle);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      // Set texture wrapping and filtering options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      // Free stb_image allocated data
      stbi_image_free(data);
    }
    else
    {
      std::cout << "Failed to load texture: " << _path << std::endl;
    }
  }

  u0
  bind()
  {
    glBindTexture(GL_TEXTURE_2D, tex_handle);
  }

  u0
  unbind()
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  i32 size_x{0}, size_y{0};
  GLuint tex_handle{0};
};
} // namespace renderer