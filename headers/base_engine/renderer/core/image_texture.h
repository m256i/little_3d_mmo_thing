#pragma once

#include "base_engine/renderer/shader.h"
#include <include/stb_image.h>
#include <GL/gl.h>
#include <glad/glad.h>

#include "lod.h"

#include <common.h>
#include <stb_image_resize.h>

#include <stb_dxt.h>

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

      puts("lmaooo");

      glGenTextures(1, &tex_handle);
      glBindTexture(GL_TEXTURE_2D, tex_handle);

      //      auto buff = compressor.compress_image(size_x, size_y, data);

      usize blocks_x    = (size_x + 6 - 1) / 6;
      usize blocks_y    = (size_y + 6 - 1) / 6;
      usize blocks_z    = 1;
      usize buffer_size = blocks_x * blocks_y * blocks_z * 16;

      // glTexImage2D(GL_TEXTURE_2D, 0, format, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, size_x, size_y, 0, buffer_size, 0);

      glGenerateMipmap(GL_TEXTURE_2D);

      // Set texture wrapping and filtering options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      // free(buff);
      //  Free stb_image allocated data
      stbi_image_free(data);
    }
    else
    {
      puts("failos");
      std::cout << "Failed to load texture: " << _path << std::endl;
    }
  }

  u0
  load_from_buf(usize _size_x, usize _size_y, char* _buffer, u32 _num_channels, bool _do_compression)
  {
    size_x = _size_x;
    size_y = _size_y;

    assert(_num_channels == 4); // ASTC only worls with rgba textures

    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);

    // glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ASTC_6x6_KHR, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, _buffer);

    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

inline void
CompressTextureDXT1(unsigned char* dest, const unsigned char* src, int width, int height)
{
  int block_size = 8; // 8 bytes per block for DXT1

  int blocks_x = (width + 3) / 4;
  int blocks_y = (height + 3) / 4;

  for (int y = 0; y < blocks_y; ++y)
  {
    for (int x = 0; x < blocks_x; ++x)
    {
      // Position in the destination buffer
      unsigned char* dest_block = dest + (y * blocks_x + x) * block_size;

      // Position in the source buffer (RGBA)
      unsigned char block[16 * 4]; // 16 pixels * 4 bytes per pixel (RGBA)
      for (int by = 0; by < 4; ++by)
      {
        for (int bx = 0; bx < 4; ++bx)
        {
          int src_x = x * 4 + bx;
          int src_y = y * 4 + by;

          // Handle edge cases where the texture isn't a multiple of 4 in size
          if (src_x < width && src_y < height)
          {
            int src_index   = (src_y * width + src_x) * 4;
            int block_index = (by * 4 + bx) * 4;

            block[block_index + 0] = src[src_index + 0]; // R
            block[block_index + 1] = src[src_index + 1]; // G
            block[block_index + 2] = src[src_index + 2]; // B
            block[block_index + 3] = src[src_index + 3]; // A
          }
          else
          {
            // If we're outside the texture bounds, pad with zeros
            int block_index        = (by * 4 + bx) * 4;
            block[block_index + 0] = 0;
            block[block_index + 1] = 0;
            block[block_index + 2] = 0;
            block[block_index + 3] = 0;
          }
        }
      }

      // Compress this 4x4 block
      stb_compress_dxt_block(dest_block, block, 0, STB_DXT_HIGHQUAL); // 0 for no alpha, STB_DXT_NORMAL mode
    }
  }
}

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

        for (int i = 0; i < new_size_x * new_size_y * 4; i += 4)
        {
          float alpha              = data[i + 3] / 255.0f;
          scaled_image_data[i]     = static_cast<unsigned char>(data[i] * alpha);     // R
          scaled_image_data[i + 1] = static_cast<unsigned char>(data[i + 1] * alpha); // G
          scaled_image_data[i + 2] = static_cast<unsigned char>(data[i + 2] * alpha); // B
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

      // auto buff = compressor.compress_image(new_size_x, new_size_y, scaled_image_data);

      // glTexImage2D(GL_TEXTURE_2D, 0, format, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

      int width      = new_size_x;
      int height     = new_size_y;
      int block_size = 8; // 8 bytes per block for DXT1

      int blocks_x = (width + 3) / 4;
      int blocks_y = (height + 3) / 4;

      int total_size                 = blocks_x * blocks_y * block_size;
      unsigned char* compressed_data = (unsigned char*)malloc(total_size);
      CompressTextureDXT1(compressed_data, scaled_image_data, new_size_x, new_size_y);
      glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, new_size_x, new_size_y, 0, total_size, compressed_data);

      glGenerateMipmap(GL_TEXTURE_2D);

      // Set texture wrapping and filtering options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      // free(buff);

      tex_handles[i] = handle;

      free(compressed_data);

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