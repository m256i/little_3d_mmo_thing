#pragma once

#include <common.h>
#include <stb_rect_pack.h>

#include <cassert>
#include <cmath>

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
    atlas_width *= 1.2;
    atlas_height *= 1.2;
    LOG(INFO) << "increased atlas size to: " << next_multiple_of_4(atlas_width) << " x " << next_multiple_of_4(atlas_height);
  }

  return {next_multiple_of_4(atlas_width), next_multiple_of_4(atlas_height)};
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
  };

  u0
  add_texture(u32 _name_hash, const u8 *const _ptr, usize _tex_w, usize _tex_h, u32 _channel_count)
  {
    assert(_ptr);
    assert(_name_hash);

    stbrp_rect new_rect;
    new_rect.id = _name_hash;
    new_rect.w  = _tex_w;
    new_rect.h  = _tex_h;

    assert(_channel_count > 0);

    texture_mappings[_name_hash] = input_texture{
        .data_pointer = _ptr, .texture_width = _tex_w, .texture_height = _tex_h, .channel_count = (u8)_channel_count, .rect = new_rect};
  }

  u8 *
  generate()
  {
    std::vector<stbrp_rect> rects;

    for (const auto &tex : texture_mappings)
    {
      rects.push_back(tex.second.rect);
    }

    auto [atlas_width, atlas_height] = detail::grow_and_pack_textures(rects, detail::estimate_atlas_size(rects));

    this->atlas_width  = atlas_width;
    this->atlas_height = atlas_height;

    assert(atlas_width && atlas_height);

    /* copy back generated rects to out mappings */
    for (const auto &rect : rects)
    {
      assert(texture_mappings.contains(rect.id));
      texture_mappings.at(rect.id).rect = rect;
    }

    if (texture_pointer)
    {
      free(texture_pointer);
    }

    texture_pointer = (u8 *)malloc(atlas_width * atlas_height * num_channels);

    LOG(INFO) << "allocated texture pointer size: " << (atlas_width * atlas_height * num_channels);

    /*
     */
    for (const auto &tex : texture_mappings)
    {
      const u8 *const texture_data = tex.second.data_pointer;
      const i32 tex_channels       = tex.second.channel_count;
      const i32 texture_width      = tex.second.texture_width;
      const i32 texture_height     = tex.second.texture_height;

      const auto &rect = tex.second.rect;

      for (i32 y = 0; y < texture_height; ++y)
      {
        for (i32 x = 0; x < texture_width; ++x)
        {
          i32 src_index = (y * texture_width + x) * tex.second.channel_count;
          i32 dst_x     = rect.x + x;
          i32 dst_y     = rect.y + y;
          i32 dst_index = (dst_y * atlas_width + dst_x) * 4;

          texture_pointer[dst_index + 0] = texture_data[src_index + 0];
          texture_pointer[dst_index + 1] = tex.second.channel_count > 1 ? texture_data[src_index + 1] : 0;
          texture_pointer[dst_index + 2] = tex.second.channel_count > 2 ? texture_data[src_index + 2] : 0;
          texture_pointer[dst_index + 3] = tex.second.channel_count > 3 ? texture_data[src_index + 3] : 254;
        }
      }
    }

    return texture_pointer;
  }

  std::pair<f32, f32>
  get_uv_map(u32 _texture_name_hash, f32 u, f32 v)
  {
    assert(texture_mappings.contains(_texture_name_hash));
    const auto &texture_map = texture_mappings[_texture_name_hash];

    const f32 rect_pack_offset_x     = texture_map.rect.x;
    const f32 rect_pack_offset_y     = texture_map.rect.y;
    const f32 rect_pack_dimensions_x = texture_map.rect.w;
    const f32 rect_pack_dimensions_y = texture_map.rect.h;

    const f32 u_offset = (f32)(rect_pack_offset_x) / atlas_width;
    const f32 v_offset = (f32)(rect_pack_offset_y) / atlas_height;
    const f32 u_scale  = (f32)(rect_pack_dimensions_x) / atlas_width;
    const f32 v_scale  = (f32)(rect_pack_dimensions_y) / atlas_height;

    /*
    fix for tiling textures
    @FIXME: fix the fix for tiling textures
    */

    bool usigned = (u < 0);
    bool vsigned = (u < 0);

    // Handle cases where the value is below -1
    while (u < 0)
    {
      u += 1.0f; // Keep adding 1 until u is non-negative
    }
    while (v < 0)
    {
      v += 1.0f; // Keep adding 1 until v is non-negative
    }

    u = std::fmod(u, 1.0f);
    v = std::fmod(v, 1.0f);

    return {1 - (u * u_scale + u_offset), 1 - (v * v_scale + v_offset)};
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

  u8 *texture_pointer;
  usize atlas_width, atlas_height;
  constexpr static u32 num_channels = 4; // texture atlases will always have 4 channels
};
} // namespace renderer::core::textures