#pragma once

#include <glm/glm.hpp>
#include <quickhull/QuickHull.hpp>
#include <vector>
#include "../renderer/mesh.h"
#include "headers/base_engine/renderer/mesh.h"
#include "quickhull/Structs/Vector3.hpp"
#include "../../common.h"

#ifdef min(x, y)
#undef min(x, y)
#else
#ifdef max(x, y)
#undef max(x, y)
#endif
#endif

struct convex_hull_t
{
  convex_hull_t() = default;
  glm::vec3 center{};
  std::vector<glm::vec3> points{};

  u0
  from_mesh(const mesh_t& _mesh)
  {
    static constexpr auto to_glm = [](const quickhull::Vector3<f32>& _v) { return glm::vec3{_v.x, _v.y, _v.z}; };
    static constexpr auto to_qh  = [](const glm::vec3& _v) { return quickhull::Vector3<f32>{_v.x, _v.y, _v.z}; };

    std::vector<glm::vec3> vertex_raw(_mesh.vertices.size());

    for (usize i = 0; i != vertex_raw.size(); ++i)
    {
      vertex_raw[i] = _mesh.vertices.at(i).position;
    }

    for (usize i = 0; i != _mesh.vertices.size(); ++i)
    {
      vertex_raw[i] = _mesh.vertices[i].position;
    }

    quickhull::QuickHull<f32> qh{};
    std::vector<quickhull::Vector3<f32>> points_raw(vertex_raw.size());

    for (auto& [i, ite] : enumerate(vertex_raw))
    {
      points_raw[i] = to_qh(ite);
    }

    auto hull = qh.getConvexHull(points_raw, true, false);

    for (const auto& vec : hull.getVertexBuffer())
    {
      points.push_back(to_glm(vec));
      center += to_glm(vec);
    }

    center /= points.size();
  }
};
