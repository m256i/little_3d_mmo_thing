#pragma once

#include <common.h>
#include <stb_rect_pack.h>

#include <cassert>
#include <cmath>
#include <glm/glm.hpp>

namespace renderer::core::textures
{

namespace detail
{
inline bool
try_pack_textures(int atlas_width, int atlas_height, std::vector<stbrp_rect> &rects)
{
  stbrp_context context;
  std::vector<stbrp_node> nodes(atlas_width);
  stbrp_init_target(&context, atlas_width, atlas_height, nodes.data(), atlas_width);
  stbrp_pack_rects(&context, rects.data(), rects.size());
  for (const auto &rect : rects)
  {
    if (!rect.was_packed)
    {
      return false;
    }
  }
  return true;
}

inline i32
next_multiple_of_4(i32 value)
{
  return (value + 3) & ~3;
}

inline std::pair<i32, i32>
grow_and_pack_textures(std::vector<stbrp_rect> &rects, std::pair<i32, i32> initial_size)
{
  i32 atlas_width  = initial_size.first;
  i32 atlas_height = initial_size.second;

  while (!try_pack_textures(atlas_width, atlas_height, rects))
  {
    atlas_width  = next_multiple_of_4(atlas_width * 1.2);
    atlas_height = next_multiple_of_4(atlas_height * 1.2);
    LOG(INFO) << "increased atlas size to: " << atlas_width << " x " << atlas_height;
  }

  return {atlas_width, atlas_height};
}

inline i32
next_power_of_two(i32 v)
{
  return std::pow(2, std::ceil(std::log2(v)));
}

inline std::pair<i32, i32>
estimate_atlas_size(const std::vector<stbrp_rect> &rects)
{
  i32 total_area = 0;

  for (const auto &rect : rects)
  {
    total_area += rect.w * rect.h;
  }

  i32 side_length = std::sqrt(total_area);
  i32 atlas_size  = next_power_of_two(side_length);

  return {atlas_size / 3, atlas_size / 3};
}
} // namespace detail

struct texture_atlas
{
  struct input_texture
  {
    const u8 *data_pointer;
    usize texture_width, texture_height;
    u8 channel_count;
    stbrp_rect rect;
    glm::vec2 min_uv;
    glm::vec2 max_uv;
  };

  u0
  add_texture(u32 _name_hash, const u8 *const _ptr, usize _tex_w, usize _tex_h, u32 _channel_count, glm::vec2 _min_uv = {0, 0},
              glm::vec2 _max_uv = {1, 1})
  {
    assert(_ptr);
    assert(_name_hash);

    /*
    @TODO: the issue is somewhere here clearly! and also in the UV remapping code
    */

    _min_uv = glm::vec2{std::floor(_min_uv.x) - 1.f, std::floor(_min_uv.y) - 1.f};
    _max_uv = glm::vec2{std::ceil(_max_uv.x) + 1.f, std::ceil(_max_uv.y) + 1.f};

    stbrp_rect new_rect;
    new_rect.id = _name_hash;

    /*
    make tiling textures work
    */
    new_rect.w = _tex_w * u32(std::abs(_min_uv.x) + std::abs(_max_uv.x));
    new_rect.h = _tex_h * u32(std::abs(_min_uv.y) + std::abs(_max_uv.y));

    assert(_channel_count > 0);

    texture_mappings[_name_hash] = input_texture{.data_pointer   = _ptr,
                                                 .texture_width  = _tex_w,
                                                 .texture_height = _tex_h,
                                                 .channel_count  = (u8)_channel_count,
                                                 .rect           = new_rect,
                                                 .min_uv         = _min_uv,
                                                 .max_uv         = _max_uv};
  }

  [[nodiscard]] u8 *
  generate()
  {
    std::vector<stbrp_rect> rects;

    for (const auto &tex : texture_mappings)
    {
      rects.push_back(tex.second.rect);
    }

    auto [atlas_width, atlas_height] = detail::grow_and_pack_textures(rects, detail::estimate_atlas_size(rects));

    LOG(INFO) << "generated atlas size";

    this->atlas_width  = atlas_width;
    this->atlas_height = atlas_height;

    assert(atlas_width && atlas_height);

    /* copy back generated rects to out mappings */
    for (const auto &rect : rects)
    {
      assert(texture_mappings.contains(rect.id));
      texture_mappings.at(rect.id).rect = rect;
    }

    LOG(INFO) << "yuh";

    if (texture_pointer)
    {
      free(texture_pointer);
    }

    texture_pointer = (u8 *)malloc(atlas_width * atlas_height * num_channels);
    std::memset(texture_pointer, 0, atlas_width * atlas_height * num_channels);

    LOG(INFO) << "allocated texture pointer size: " << (atlas_width * atlas_height * num_channels);

    /*
     */
    for (const auto &tex : texture_mappings)
    {
      const u8 *const texture_data = tex.second.data_pointer;
      const i32 tex_channels       = tex.second.channel_count;

      const i32 texture_repeats_x = std::abs(tex.second.min_uv.x - tex.second.max_uv.x);
      const i32 texture_repeats_y = std::abs(tex.second.min_uv.y - tex.second.max_uv.y);

      // LOG(INFO) << "texture_repeats_x " << texture_repeats_x;
      // LOG(INFO) << "texture_repeats_y " << texture_repeats_y;

      const i32 texture_width  = tex.second.texture_width * std::max(texture_repeats_x, 1);
      const i32 texture_height = tex.second.texture_height * std::max(texture_repeats_y, 1);

      const i32 adjusted_rect_x = atlas_width - texture_width;
      const auto &rect          = tex.second.rect;

      for (i32 y = 0; y < texture_height; ++y)
      {
        for (i32 x = 0; x < texture_width; ++x)
        {
          i32 src_index =
              ((y % tex.second.texture_height) * tex.second.texture_width + (x % tex.second.texture_width)) * tex.second.channel_count;

          // LOG(INFO) << "source index" << src_index;
          //  i32 dst_x     = rect.x + x;
          i32 dst_x = rect.x + ((texture_width)-1 - x);
          // i32 dst_y = rect.y + ((texture_height)-1 - y);
          i32 dst_y     = rect.y + y;
          i32 dst_index = (dst_y * atlas_width + dst_x) * 4;

          texture_pointer[dst_index + 0] = texture_data[src_index + 0];
          texture_pointer[dst_index + 1] = tex.second.channel_count > 1 ? texture_data[src_index + 1] : 0;
          texture_pointer[dst_index + 2] = tex.second.channel_count > 2 ? texture_data[src_index + 2] : 0;
          texture_pointer[dst_index + 3] = tex.second.channel_count > 3 ? texture_data[src_index + 3] : 255;
        }
      }
    }

    LOG(INFO) << "generating done!";

    debug_texture_pointer = (u8 *)malloc(atlas_width * atlas_height * num_channels);
    std::memset(debug_texture_pointer, 0, atlas_width * atlas_height * num_channels);
    std::memcpy(debug_texture_pointer, texture_pointer, atlas_width * atlas_height * num_channels);

    return texture_pointer;
  }

  u0
  add_debug_triangle(glm::vec2 triA, glm::vec2 triB, glm::vec2 triC)
  {
    assert(debug_texture_pointer);
    add_triangle_edges(debug_texture_pointer, glm::ivec2(atlas_width, atlas_height), triA, triB, triC);
  }

  u8 *
  get_debug_texture()
  {
    return debug_texture_pointer;
  }

  // Helper function to draw a line between two points using Bresenham's algorithm
  void
  draw_line(u8 *image_buffer, glm::ivec2 image_size, glm::ivec2 start, glm::ivec2 end, int num_channels)
  {
    int x0 = start.x, y0 = start.y;
    int x1 = end.x, y1 = end.y;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true)
    {
      // Ensure pixel is inside image bounds
      if (x0 >= 0 && x0 < image_size.x && y0 >= 0 && y0 < image_size.y)
      {
        // Calculate the index in the image buffer
        int index = (y0 * image_size.x + x0) * num_channels;

        // Invert the color of the pixel (RGB channels only)
        image_buffer[index + 0] = 255 - image_buffer[index + 0]; // Red channel
        image_buffer[index + 1] = 255 - image_buffer[index + 1]; // Green channel
        image_buffer[index + 2] = 255 - image_buffer[index + 2]; // Blue channel
                                                                 // Alpha channel (image_buffer[index + 3]) remains unchanged
      }

      if (x0 == x1 && y0 == y1) break;

      int e2 = 2 * err;

      if (e2 > -dy)
      {
        err -= dy;
        x0 += sx;
      }

      if (e2 < dx)
      {
        err += dx;
        y0 += sy;
      }
    }
  }

  // Function to map UV coordinates and draw lines on the texture
  void
  add_triangle_edges(u8 *image_buffer, glm::ivec2 image_size, glm::vec2 triA, glm::vec2 triB, glm::vec2 triC)
  {
    const int num_channels = 4; // Assuming RGBA format with 4 channels

    // Convert UV coordinates (0,0)-(1,1) to pixel coordinates (image space)
    glm::ivec2 pixelA = glm::ivec2(triA * glm::vec2(image_size));
    glm::ivec2 pixelB = glm::ivec2(triB * glm::vec2(image_size));
    glm::ivec2 pixelC = glm::ivec2(triC * glm::vec2(image_size));

    // Draw the edges of the triangle by connecting the vertices
    draw_line(image_buffer, image_size, pixelA, pixelB, num_channels); // Line AB
    draw_line(image_buffer, image_size, pixelB, pixelC, num_channels); // Line BC
    draw_line(image_buffer, image_size, pixelC, pixelA, num_channels); // Line CA
  }

  std::pair<f32, f32>
  get_uv_map(u32 _texture_name_hash, f32 u, f32 v)
  {
    assert(texture_mappings.contains(_texture_name_hash));
    const auto &texture_map = texture_mappings[_texture_name_hash];

    // LOG(INFO) << "get uv map for texture: " << _texture_name_hash << " u" << u << " v" << v;

    /*
    BUG#1:  atlas_width and height are oversized because the texture has to be a multiple of 4

    */

    /*
    the offset of the mapped texture in the atlas
    */
    const f32 rect_pack_offset_x = texture_map.rect.x;
    const f32 rect_pack_offset_y = texture_map.rect.y;

    /*
    the size of a single texture
    */
    const f32 rect_pack_dimensions_x = texture_map.texture_width;
    const f32 rect_pack_dimensions_y = texture_map.texture_height;

    /*
    the UV coordinate offsets passed to shader basically just raw offsets normalized
    */
    const f32 u_offset = (f32)(rect_pack_offset_x) / atlas_width;
    const f32 v_offset = (f32)(rect_pack_offset_y) / atlas_height;

    /*
    the scale of the new UVs to scale the old UVs with
     */
    const f32 u_scale = (f32)(rect_pack_dimensions_x) / atlas_width;
    const f32 v_scale = (f32)(rect_pack_dimensions_y) / atlas_height;

    // LOG(INFO) << "atlas x: " << atlas_width << "atlas y: " << atlas_height;
    LOG(INFO) << "u_scale: " << u_scale << " v_scale: " << v_scale;
    LOG(INFO) << "u_offset: " << u_offset << " v_offset: " << v_offset;

    LOG(INFO) << "with UVminY: " << texture_map.min_uv.y;
    LOG(INFO) << "with UVminX: " << texture_map.min_uv.x;

    LOG(INFO) << "original U: " << u;
    LOG(INFO) << "original V: " << v;

    if (texture_map.min_uv.x < 0)
    {
      u += (std::abs(std::floor(texture_map.min_uv.x)));
    }
    else if (texture_map.min_uv.x > 0)
    {
      u -= (std::ceil(texture_map.min_uv.x));
    }

    if (texture_map.min_uv.y < 0)
    {
      v += std::abs(std::floor(texture_map.min_uv.y));
    }
    else if (texture_map.min_uv.y > 0)
    {
      v -= std::ceil(texture_map.min_uv.y);
    }

    LOG(INFO) << "adjusted U: " << u;
    LOG(INFO) << "adjusted V: " << v;
    LOG(INFO) << " mapped u: " << u << " v: " << v << " to " << (u * u_scale + u_offset) << " " << (v * v_scale + v_offset);

    return {(u * u_scale + u_offset), (v * v_scale + v_offset)};
  }

  u0
  destroy()
  {
    texture_mappings.clear();
    if (texture_pointer)
    {
      free(texture_pointer);
    }
  }

  std::unordered_map<u32, input_texture> texture_mappings;

  u8 *texture_pointer{nullptr}, *debug_texture_pointer{nullptr};
  usize atlas_width, atlas_height;
  constexpr static u32 num_channels = 4; // texture atlases will always have 4 channels
};
} // namespace renderer::core::textures