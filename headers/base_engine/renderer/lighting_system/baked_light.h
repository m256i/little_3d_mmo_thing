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

  inline glm::vec2
  tritrimap(const glm::vec2& P, const glm::vec2& A, const glm::vec2& B, const glm::vec2& C, const glm::vec2& A1, const glm::vec2& B1,
            const glm::vec2& C1)
  {
    glm::vec3 bary = barycentric(P, A, B, C);
    if (bary.x > 1 || bary.y > 1 || bary.z > 1)
    {
      return {std::numeric_limits<f32>::quiet_NaN(), std::numeric_limits<f32>::quiet_NaN()};
    }
    return bary.x * A1 + bary.y * B1 + bary.z * C1;
  }

  tri_shade_t
  shade_triangle(const triangle& _tri)
  {
    tri_shade_t out;

    usize tex_w = 20, tex_h = 20;
    out.pixel_data.resize(tex_w * tex_h);

    out.tex_w = tex_w;
    out.tex_h = tex_h;

    static auto texture_at = [&](usize x, usize y) -> u32&
    {
      // map 3d input triangle coord to texture tri
      glm::vec2 pixel_coord = tritrimap(glm::vec2{x, y}, {0, 0}, {0, 1}, {1, 0}, {0, 0}, {0, 1}, {1, 0});
      return (out.pixel_data[x * pixel_coord.x * tex_w + y * pixel_coord.y]);
    };

    out.uvC = glm::vec2(0, 0);
    out.uvB = glm::vec2(0, 1);
    out.uvA = glm::vec2(1, 0);

    const auto& tri_norm = _tri.normal;

    puts("asdasd");

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

        const auto vertex_coord = trimap(_tri, tex_uv); // map texture pixel to point on triangle

        glm::vec4 color_accum{};

        /*
        accumulate every light source
        */
        for (auto& light_source : light_sources)
        {
          f32 brightness = glm::dot(glm::normalize(vertex_coord - light_source.position), tri_norm);
          brightness     = std::clamp(brightness, 0.f, 1.f);
          // LOG(INFO) << "brightness for current vert: " << brightness;
          f32 amplitude = 1.f / (std::pow(glm::distance(vertex_coord, light_source.position), 2.5f) + FLT_EPSILON);
          amplitude     = std::clamp(amplitude, 0.f, 1.f);
          // LOG(INFO) << "amplitude for current vert: " << amplitude;
          color_accum += rgba_to_vec4(light_source.color) * brightness * amplitude * 1000.f;
        }

        // current_pixel = vec4_to_rgba(glm::vec4(tri_norm, 1.f));

        current_pixel = vec4_to_rgba(color_accum);
      }
    }
    return out;
  }
};

} // namespace renderer::lighting::static_lighting