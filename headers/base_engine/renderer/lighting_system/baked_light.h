#pragma once

#include <cmath>
#include <common.h>
#include <glm/glm.hpp>
#include <limits>

namespace renderer::lighting::static_lighting
{

struct static_point_light_t
{
  glm::vec3 position;
  u32 color;
  f32 radius;
};

struct static_lighting_t
{
  std::vector<static_point_light_t> light_sources;

  struct triangle
  {
    glm::vec3 a, b, c, normal;
  };

  struct triangle2d
  {
    glm::vec2 a, b, c;
  };

  struct tri_shade_t
  {
    std::vector<u32> pixel_data;
    usize tex_w, tex_h;
    glm::vec2 uvA, uvB, uvC;
  };

  glm::vec3
  trimap(const triangle& _tri, glm::vec2 _uv)
  {
    return (_tri.a + ((_tri.c - _tri.a) - (_tri.c - _tri.b) * _uv.y) * _uv.x);
  }

  glm::vec2
  trimap(const triangle2d& _tri, glm::vec2 _uv)
  {
    return (_tri.a + ((_tri.c - _tri.a) - (_tri.c - _tri.b) * _uv.y) * _uv.x);
  }

  inline glm::vec3
  barycentric(const glm::vec2& P, const glm::vec2& A, const glm::vec2& B, const glm::vec2& C)
  {
    glm::vec2 v0 = B - A;
    glm::vec2 v1 = C - A;
    glm::vec2 v2 = P - A;

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;
    float v     = (d11 * d20 - d01 * d21) / denom;
    float w     = (d00 * d21 - d01 * d20) / denom;
    float u     = 1.0f - v - w;

    return glm::vec3(u, v, w);
  }

  inline glm::vec3
  tritrimap(const glm::vec2& P, const glm::vec2& A, const glm::vec2& B, const glm::vec2& C, const glm::vec3& A1, const glm::vec3& B1,
            const glm::vec3& C1)
  {
    // Get barycentric coordinates for the 2D point P in the UV triangle
    glm::vec3 bary = barycentric(P, A, B, C);

    if (bary.x < 0.0 || bary.y < 0.0 || bary.z < 0.0)
    {
      return glm::vec3{std::numeric_limits<f32>::signaling_NaN()};
    }

    if (bary.y + bary.z > 1.0)
    {
      return glm::vec3{std::numeric_limits<f32>::signaling_NaN()};
    }

    return bary.x * A1 + bary.y * B1 + bary.z * C1;
  }

  inline glm::vec3
  calculate_normal(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C)
  {
    // Calculate the two edge vectors
    glm::vec3 edge1 = B - A;
    glm::vec3 edge2 = C - A;

    // Compute the cross product of the two edges
    glm::vec3 normal = glm::cross(edge1, edge2);

    // Normalize the resulting vector to make it a unit normal
    return glm::normalize(normal);
  }

  tri_shade_t
  shade_triangle(const triangle& _tri)
  {
    tri_shade_t out;

    usize tex_w = 20, tex_h = 20;
    out.pixel_data.resize(tex_w * tex_h);

    out.tex_w = tex_w;
    out.tex_h = tex_h;

    static auto texture_at = [&](usize x, usize y) -> u32& { return (out.pixel_data[y * tex_w + (tex_h - x - 1)]); };

    out.uvA = glm::vec2(1, 1);
    out.uvB = glm::vec2(1, 0);
    out.uvC = glm::vec2(0, 1);

    const auto& tri_norm = glm::normalize(calculate_normal(_tri.a, _tri.b, _tri.c));

    /*
    we iterate over every pixel of the texture and find the corresponding UV point of the triangle
    */

    static constexpr auto rgba_to_vec4 = [](u32 _col)
    {
      return glm::vec4{(f32)((_col & 0xff000000) >> 24) / 255.f, (f32)((_col & 0x00ff0000) >> 16) / 255.f,
                       (f32)((_col & 0x0000ff00) >> 8) / 255.f, (f32)((_col & 0x000000ff)) / 255.f};
    };

    static constexpr auto vec4_to_rgba = [](glm::vec4 _col)
    {
      const u8 r = (u8)(_col.r * 255);
      const u8 g = (u8)(_col.g * 255);
      const u8 b = (u8)(_col.b * 255);
      const u8 a = (u8)(_col.a * 255);

      return (u32)((u32)(r << 24) | (u32)(g << 16) | (u32)(b << 8) | (u32)(a));
    };

    for (usize texX = 0; texX < tex_w; texX++)
    {
      for (usize texY = 0; texY < tex_h; texY++)
      {
        auto& current_pixel = texture_at(texX, texY);
        glm::vec2 tex_uv((f32)texX / (f32)tex_w, (f32)texY / (f32)tex_h);

        auto vertex_coord = tritrimap(tex_uv, out.uvA, out.uvB, out.uvC, _tri.a, _tri.b, _tri.c); // map texture pixel to point on triangle

        if (glm::any(glm::isnan(vertex_coord)))
        {
          continue;
        }

        vertex_coord = (_tri.a + _tri.b + _tri.c) / 3.f;

        glm::vec4 color_accum{};

        // color_accum.r = glm::clamp(tri_norm.x, 0.f, 1.0f);
        // color_accum.g = glm::clamp(tri_norm.y, 0.f, 1.0f);
        // color_accum.b = glm::clamp(tri_norm.z, 0.f, 1.0f);
        // color_accum.a = 1;

        /*
        accumulate every light source
        */
        for (auto& light_source : light_sources)
        {
          f32 brightness = glm::dot(glm::normalize(vertex_coord - light_source.position), tri_norm);
          brightness     = std::clamp(brightness, 0.f, 1.f);
          // LOG(INFO) << "brightness for current vert: " << brightness;
          f32 amplitude = 1.f / (glm::distance(vertex_coord, light_source.position) + FLT_EPSILON);
          amplitude     = std::clamp(amplitude, 0.f, 1.f);
          // LOG(INFO) << "amplitude for current vert: " << amplitude;
          color_accum += rgba_to_vec4(light_source.color) * amplitude * 3.f;
        }

        // current_pixel = vec4_to_rgba(glm::vec4(tri_norm, 1.f));

        current_pixel = vec4_to_rgba(color_accum);
      }
    }
    return out;
  }
};

} // namespace renderer::lighting::static_lighting