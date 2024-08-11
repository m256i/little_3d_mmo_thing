#pragma once

#include "base_engine/renderer/shader.h"
#include <include/stb_image.h>
#include <GL/gl.h>
#include <glad/glad.h>

#include "lod.h"

#include <common.h>
#include <stb_image_resize.h>

namespace renderer
{
struct image_tex
{

  /* TODO handle differnt channel types */
  u0
  load(const char* _path)
  {
    int channels;
    unsigned char* data = stbi_load(_path, &size_x, &size_y, &channels, STBI_rgb);

    if (data)
    {

      u32 format{0};
      switch (channels)
      {
      case 1:
      {
        format = (u32)GL_RED;
        break;
      }
      case 3:
      {
        format = (u32)GL_RGB;
        break;
      }
      case 4:
      {
        format = (u32)GL_RGBA;
        for (int i = 0; i < size_x * size_y * 4; i += 4)
        {
          float alpha = data[i + 3] / 255.0f;
          data[i]     = static_cast<unsigned char>(data[i] * alpha);     // R
          data[i + 1] = static_cast<unsigned char>(data[i + 1] * alpha); // G
          data[i + 2] = static_cast<unsigned char>(data[i + 2] * alpha); // B
                                                                         // data[i + 3] is already the alpha channel
        }
        break;
      }
      default:
      {
        break;
      }
      }

      glGenTextures(1, &tex_handle);
      glBindTexture(GL_TEXTURE_2D, tex_handle);

      glTexImage2D(GL_TEXTURE_2D, 0, format, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

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
      puts("failos");
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

struct image_tex_lod
{

  u0
  load(const char* _path, std::string_view texture_type = "texture_diffuse")
  {
    int channels;

    unsigned char* data = stbi_load(_path, &size_x, &size_y, &channels, STBI_rgb_alpha);

    if (!data)
    {
      std::cout << "Failed to load texture: " << _path << std::endl;
      assert(false);
    }

    for (usize i = 0; i < lod::detail_scales.size(); i++)
    {
      f32 scale_factor = lod::detail_scales[i];

      u32 new_size_x = static_cast<u32>(static_cast<f32>(size_x) * scale_factor);
      u32 new_size_y = static_cast<u32>(static_cast<f32>(size_y) * scale_factor);

      if (new_size_x == 0 || new_size_y == 0)
      {
        std::cerr << "new dimensions are zero. skipping this LOD level." << std::endl;
        continue;
      }

      unsigned char* scaled_image_data = new unsigned char[new_size_x * new_size_y * channels];

      if (!scaled_image_data)
      {
        puts("memoory allocation failed wtf!");
        assert(false);
      }

      if (!stbir_resize_uint8(data, size_x, size_y, 0, scaled_image_data, new_size_x, new_size_y, 0, channels))
      {
        printf("failed to resize image at LOD level: %d\n", i);
        delete[] scaled_image_data;
        continue;
      }

      GLuint handle = 0;

      u32 format{0};
      switch (channels)
      {
      case 1:
      {
        format = (u32)GL_RED;
        break;
      }
      case 3:
      {
        format = (u32)GL_RGB;
        break;
      }
      case 4:
      {
        format = (u32)GL_RGBA;

        for (int i = 0; i < size_x * size_y * 4; i += 4)
        {
          float alpha = data[i + 3] / 255.0f;
          data[i]     = static_cast<unsigned char>(data[i] * alpha);     // R
          data[i + 1] = static_cast<unsigned char>(data[i + 1] * alpha); // G
          data[i + 2] = static_cast<unsigned char>(data[i + 2] * alpha); // B
                                                                         // data[i + 3] is already the alpha channel
        }

        break;
      }
      default:
      {
        break;
      }
      }

      glGenTextures(1, &handle);
      glBindTexture(GL_TEXTURE_2D, handle);
      glTexImage2D(GL_TEXTURE_2D, 0, format, new_size_x, new_size_y, 0, format, GL_UNSIGNED_BYTE, scaled_image_data);
      glGenerateMipmap(GL_TEXTURE_2D);

      // Set texture wrapping and filtering options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      tex_handles[i] = handle;

      // Free stb_image allocated data
      delete[] scaled_image_data;
    }

    type = texture_type;
    path = _path;

    stbi_image_free(data);
  }

  /* bind texture with certain LOD level. 0 means full resoution. higher number -> lower res texture */
  u0
  bind(lod::detail_level lod_level) const
  {
    assert((u32)lod_level < lod::detail_scales.size());
    auto handle = tex_handles.at((u32)lod_level);
    glBindTexture(GL_TEXTURE_2D, handle);
  }

  glm::vec2
  get_lod_size(lod::detail_level lod_level) const
  {
    return {(f32)size_x * lod::detail_scales[(u32)lod_level], (f32)size_y * lod::detail_scales[(u32)lod_level]};
  }

  u0
  unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  i32 size_x{0}, size_y{0};
  std::array<GLuint, (usize)lod::detail_level::lod_detail_enum_size> tex_handles{};
  std::string type, path;
};

} // namespace renderer