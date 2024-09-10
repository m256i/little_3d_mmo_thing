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

/*
RGBA input RGB output
*/
inline void
compress_rgba_dxt1(u8* const __restrict _dst, const u8* const __restrict _src, usize width, usize height)
{
  constexpr static usize block_size = 8; // 8 bytes per block for DXT1
  const usize blocks_x              = (width + 3) / 4;
  const usize blocks_y              = (height + 3) / 4;
  for (usize y = 0; y < blocks_y; ++y)
  {
    for (usize x = 0; x < blocks_x; ++x)
    {
      u8* dest_block = _dst + (y * blocks_x + x) * block_size;
      u8 block[16 * 4]; // 16 pixels * 4 bytes per pixel (RGBA)
      for (usize by = 0; by < 4; ++by)
      {
        for (usize bx = 0; bx < 4; ++bx)
        {
          const usize src_x = x * 4 + bx;
          const usize src_y = y * 4 + by;
          // handle cases where the texture isn't a multiple of 4 in size
          if (src_x < width && src_y < height)
          {
            usize src_index        = (src_y * width + src_x) * 4;
            usize block_index      = (by * 4 + bx) * 4;
            block[block_index + 0] = _src[src_index + 0];
            block[block_index + 1] = _src[src_index + 1];
            block[block_index + 2] = _src[src_index + 2];
            block[block_index + 3] = _src[src_index + 3];
          }
          else
          {
            // OOB pad with zeros
            int block_index        = (by * 4 + bx) * 4;
            block[block_index + 0] = 0;
            block[block_index + 1] = 0;
            block[block_index + 2] = 0;
            block[block_index + 3] = 0;
          }
        }
      }
      // Compress this 4x4 block
      stb_compress_dxt_block(dest_block, block, 0, STB_DXT_NORMAL); // 0 for no alpha, STB_DXT_NORMAL mode
    }
  }
}

inline void
compress_rgba_dxt3(u8* const __restrict _dst, const u8* const __restrict _src, usize width, usize height)
{
  constexpr static usize block_size = 16; // 16 bytes per block for DXT3

  const usize blocks_x = (width + 3) / 4;  // Calculate the number of blocks in the x direction
  const usize blocks_y = (height + 3) / 4; // Calculate the number of blocks in the y direction

  for (usize y = 0; y < blocks_y; ++y)
  {
    for (usize x = 0; x < blocks_x; ++x)
    {
      u8* dest_block = _dst + (y * blocks_x + x) * block_size; // Destination block for compressed data
      u8 block[16 * 4];                                        // 16 pixels * 4 bytes per pixel (RGBA)

      for (usize by = 0; by < 4; ++by)
      {
        for (usize bx = 0; bx < 4; ++bx)
        {
          const usize src_x = x * 4 + bx; // Calculate the source x position
          const usize src_y = y * 4 + by; // Calculate the source y position

          // Handle cases where the texture isn't a multiple of 4 in size
          if (src_x < width && src_y < height)
          {
            usize src_index   = (src_y * width + src_x) * 4; // Source index for RGBA
            usize block_index = (by * 4 + bx) * 4;           // Destination index for RGBA

            // Copy RGBA values
            block[block_index + 0] = _src[src_index + 0]; // R
            block[block_index + 1] = _src[src_index + 1]; // G
            block[block_index + 2] = _src[src_index + 2]; // B
            block[block_index + 3] = _src[src_index + 3]; // A
          }
          else
          {
            // OOB pad with zeros
            int block_index        = (by * 4 + bx) * 4;
            block[block_index + 0] = 0; // R
            block[block_index + 1] = 0; // G
            block[block_index + 2] = 0; // B
            block[block_index + 3] = 0; // A
          }
        }
      }

      // Prepare the DXT3 block
      u8 dxt3_block[16]; // 16 bytes for the DXT3 block

      // Copy alpha values
      for (usize i = 0; i < 4; ++i)
      {
        // Each 4 pixels get 4 bits of alpha, hence 2 bytes for each row of 4 pixels
        dxt3_block[i * 2]     = block[i * 4 + 3]; // Alpha value for pixel 0, 1, 2, 3
        dxt3_block[i * 2 + 1] = block[i * 4 + 3]; // Copy same alpha value for each pixel (DXT3 uses 4 bits for each pixel)
      }

      // Compress RGB data
      stb_compress_dxt_block(dest_block, block, 0, STB_DXT_NORMAL); // Compress RGB values (stb_dxt handles DXT3 format)
      // Combine the alpha and RGB data into the final DXT3 block
      memcpy(dest_block, dxt3_block, 8);     // First 8 bytes for alpha
      memcpy(dest_block + 8, dest_block, 8); // Next 8 bytes for RGB

      // Compress the RGB data into the DXT3 block
      stb_compress_dxt_block(dest_block + 8, block, 0, STB_DXT_NORMAL);
    }
  }
}

inline void
compress_rgba_dxt5(u8* const __restrict _dst, const u8* const __restrict _src, usize width, usize height)
{
  constexpr static usize block_size = 16; // 16 bytes per block for DXT5

  const usize blocks_x = (width + 3) / 4;  // Calculate the number of blocks in the x direction
  const usize blocks_y = (height + 3) / 4; // Calculate the number of blocks in the y direction

  for (usize y = 0; y < blocks_y; ++y)
  {
    for (usize x = 0; x < blocks_x; ++x)
    {
      u8* dest_block = _dst + (y * blocks_x + x) * block_size; // Destination block for compressed data
      u8 block[16 * 4];                                        // 16 pixels * 4 bytes per pixel (RGBA)

      // Extract the 4x4 block from the image
      for (usize by = 0; by < 4; ++by)
      {
        for (usize bx = 0; bx < 4; ++bx)
        {
          const usize src_x = x * 4 + bx; // Calculate the source x position
          const usize src_y = y * 4 + by; // Calculate the source y position

          // Handle cases where the texture isn't a multiple of 4 in size
          if (src_x < width && src_y < height)
          {
            usize src_index   = (src_y * width + src_x) * 4; // Source index for RGBA
            usize block_index = (by * 4 + bx) * 4;           // Destination index for RGBA

            // Copy RGBA values
            block[block_index + 0] = _src[src_index + 0]; // R
            block[block_index + 1] = _src[src_index + 1]; // G
            block[block_index + 2] = _src[src_index + 2]; // B
            block[block_index + 3] = _src[src_index + 3]; // A
          }
          else
          {
            // OOB pad with zeros
            int block_index        = (by * 4 + bx) * 4;
            block[block_index + 0] = 0; // R
            block[block_index + 1] = 0; // G
            block[block_index + 2] = 0; // B
            block[block_index + 3] = 0; // A
          }
        }
      }

      // Compress the alpha channel for DXT5
      stb_compress_dxt_block(dest_block, block, 1, 2); // Alpha flag = 1, mode = 2 for DXT5

      // Compress the RGB data
      stb_compress_dxt_block(dest_block + 8, block, 0, STB_DXT_NORMAL); // No alpha flag for RGB data
    }
  }
}

struct image_tex
{
  /* TODO handle differnt channel types */
  u0
  load(const char* _path, std::string_view _name = "texture_diffuse")
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
        break;
      }
      default:
      {
        break;
      }
      }

      glGenTextures(1, &tex_handle);
      glBindTexture(GL_TEXTURE_2D, tex_handle);

      if (format == GL_RGB)
      {
        const usize total_size = ((size_x + 3) / 4) * ((size_y + 3) / 4) * 8ull; // for rgb output DXT1
        u8* compressed_data    = (u8*)malloc(total_size);
        compress_rgba_dxt1(compressed_data, data, size_x, size_y);
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, size_x, size_y, 0, total_size, compressed_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(compressed_data);
      }
      else if (format == GL_RGBA)
      {
        const usize total_size = ((size_x + 3) / 4) * ((size_y + 3) / 4) * 16ull; // for rgba output DXT3
        u8* compressed_data    = (u8*)malloc(total_size);
        compress_rgba_dxt5(compressed_data, data, size_x, size_y);
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, size_x, size_y, 0, total_size, compressed_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(compressed_data);
      }
      else
      {
        glTexImage2D(GL_TEXTURE_2D, 0, format, size_x, size_y, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }

      name = _name;

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
  bind() const
  {
    glBindTexture(GL_TEXTURE_2D, tex_handle);
  }

  u0
  unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  std::string_view name;
  i32 size_x{0}, size_y{0};
  GLuint tex_handle{0};
};

struct image_tex_lod
{
  image_tex_lod() = default;
  image_tex_lod(const char* _name, const char* _path) : path(_path), name(_name) {}

  u0
  load(const char* _path, std::string_view name = "", std::string_view texture_type = "texture_diffuse")
  {
    i32 channels;

    u8* data = stbi_load(_path, &size_x, &size_y, &channels, STBI_rgb_alpha);

    if (!data)
    {
      std::cout << "Failed to load texture: " << _path << std::endl;
      assert(false);
    }

    for (usize i = 0; i < lod::detail_scales.size(); i++)
    {
      const f32 scale_factor = lod::detail_scales[i];
      const u32 new_size_x   = (u32)((f32)(size_x)*scale_factor);
      const u32 new_size_y   = (u32)((f32)(size_y)*scale_factor);

      if (new_size_x == 0 || new_size_y == 0)
      {
        std::cerr << "new dimensions are zero. skipping this LOD level." << std::endl;
        continue;
      }

      u8* scaled_image_data = new u8[new_size_x * new_size_y * channels];

      if (!stbir_resize_uint8(data, size_x, size_y, 0, scaled_image_data, new_size_x, new_size_y, 0, channels))
      {
        printf("failed to resize image at LOD level: %d\n", i);
        delete[] scaled_image_data;
        continue;
      }

      u32 handle{0};
      u32 format{0};

      switch (channels)
      {
      case 1:
      {
        format = (u32)GL_RED;
        break;
      }
      case 2:
      {
        format = (u32)GL_RG;
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
        break;
      }
      default:
      {
        break;
      }
      }

      glGenTextures(1, &handle);
      glBindTexture(GL_TEXTURE_2D, handle);

      if (format == GL_RGB)
      {
        const usize total_size = ((new_size_x + 3) / 4) * ((new_size_y + 3) / 4) * 8ull; // for rgb output DXT1
        u8* compressed_data    = (u8*)malloc(total_size);
        compress_rgba_dxt1(compressed_data, scaled_image_data, new_size_x, new_size_y);
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, new_size_x, new_size_y, 0, total_size, compressed_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(compressed_data);
      }
      else if (format == GL_RGBA)
      {
        const usize total_size = ((new_size_x + 3) / 4) * ((new_size_y + 3) / 4) * 16ull; // for rgba output DXT3
        u8* compressed_data    = (u8*)malloc(total_size);
        compress_rgba_dxt3(compressed_data, scaled_image_data, new_size_x, new_size_y);

        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, new_size_x, new_size_y, 0, total_size, compressed_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(compressed_data);
      }
      else
      {
        glTexImage2D(GL_TEXTURE_2D, 0, format, new_size_x, new_size_y, 0, format, GL_UNSIGNED_BYTE, scaled_image_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }

      tex_handles[i] = handle;
      delete[] scaled_image_data;
    }

    type       = texture_type;
    path       = _path;
    this->name = name;

    stbi_image_free(data);
  }

  u0
  set_lod_level(lod::detail_level level)
  {
    current_detail_level = level;
  }

  lod::detail_level
  get_lod_level() const
  {
    return current_detail_level;
  }

  u0
  load_from_mem(const u8* const _buf, usize _size_x, usize _size_y, i32 _channels, std::string_view texture_type = "texture_diffuse")
  {
    const i32 channels   = _channels;
    const u8* const data = _buf;

    size_x = _size_x;
    size_y = _size_y;

    for (usize i = 0; i < lod::detail_scales.size(); i++)
    {
      const f32 scale_factor = lod::detail_scales[i];
      const u32 new_size_x   = (u32)((f32)(size_x)*scale_factor);
      const u32 new_size_y   = (u32)((f32)(size_y)*scale_factor);

      if (new_size_x == 0 || new_size_y == 0)
      {
        std::cerr << "new dimensions are zero. skipping this LOD level." << std::endl;
        assert(false);

        continue;
      }

      u8* scaled_image_data = new u8[new_size_x * new_size_y * channels];

      if (!stbir_resize_uint8(data, size_x, size_y, 0, scaled_image_data, new_size_x, new_size_y, 0, channels))
      {
        printf("failed to resize image at LOD level: %d\n", i);
        delete[] scaled_image_data;
        continue;
      }

      u32 handle{0};
      u32 format{0};

      switch (channels)
      {
      case 1:
      {
        format = (u32)GL_RED;
        break;
      }
      case 2:
      {
        format = (u32)GL_RG;
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
        break;
      }
      default:
      {
        break;
      }
      }

      glGenTextures(1, &handle);
      glBindTexture(GL_TEXTURE_2D, handle);

      if (format == GL_RGB)
      {
        const usize total_size = ((new_size_x + 3) / 4) * ((new_size_y + 3) / 4) * 8ull; // for rgb output DXT1
        u8* compressed_data    = (u8*)malloc(total_size);
        compress_rgba_dxt1(compressed_data, scaled_image_data, new_size_x, new_size_y);
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, new_size_x, new_size_y, 0, total_size, compressed_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(compressed_data);
        puts("loaded rgb texture");
      }
      else if (format == GL_RGBA) // the compress_rgba_dxt3 function needs fixing
      {
        const usize total_size = ((new_size_x + 3) / 4) * ((new_size_y + 3) / 4) * 16ull; // for rgba output DXT3
        u8* compressed_data    = (u8*)malloc(total_size);
        compress_rgba_dxt5(compressed_data, scaled_image_data, new_size_x, new_size_y);

        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, new_size_x, new_size_y, 0, total_size, compressed_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(compressed_data);
        puts("loaded rgba texture");
      }
      else
      {
        glTexImage2D(GL_TEXTURE_2D, 0, format, new_size_x, new_size_y, 0, format, GL_UNSIGNED_BYTE, scaled_image_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }

      printf("get handle: %d", handle);

      tex_handles[i] = handle;
      delete[] scaled_image_data;
    }

    type = texture_type;
    path = "";
  }

  /* bind texture with certain LOD level. 0 means full resoution. higher number -> lower res texture */
  u0
  bind(lod::detail_level lod_level) const
  {
    assert((u32)lod_level < lod::detail_scales.size());
    auto handle = tex_handles.at((u32)lod_level);
    assert(handle);
    glBindTexture(GL_TEXTURE_2D, handle);
  }

  u0
  bind() const
  {
    assert(!tex_handles.empty());
    assert((u32)current_detail_level < lod::detail_scales.size());
    auto handle = tex_handles.at((u32)current_detail_level);
    assert(handle);
    glBindTexture(GL_TEXTURE_2D, handle);
  }

  u0
  destroy()
  {
    for (const auto tex_handle : tex_handles)
    {
      if (tex_handle)
      {
        glDeleteTextures(1, &tex_handle);
      }
    }
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

  image_tex_lod(const image_tex_lod& _other)
      : type(_other.type), path(_other.path), name(_other.name), tex_handles(_other.tex_handles),
        current_detail_level(_other.current_detail_level), size_x(_other.size_x), size_y(_other.size_y)
  {
  }

  decltype(auto)
  operator=(const image_tex_lod& _other)
  {
    type                 = _other.type;
    path                 = _other.path;
    name                 = _other.name;
    tex_handles          = _other.tex_handles;
    current_detail_level = _other.current_detail_level;
    size_x               = _other.size_x;
    size_y               = _other.size_y;
  }

  std::string type, path, name;
  std::array<GLuint, (usize)lod::detail_level::lod_detail_enum_size> tex_handles{};
  lod::detail_level current_detail_level{lod::detail_level::lod_detail_full};
  i32 size_x{0}, size_y{0};
};

} // namespace renderer