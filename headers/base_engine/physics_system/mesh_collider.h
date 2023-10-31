#pragma once

#include <array>
#include <float.h>
#include <vector>
#include <glm/vec3.hpp>
#include <deque>

#include "bbox.h"
#include "../renderer/mesh.h"
#include "glm/ext/vector_float3.hpp"
#include "headers/base_engine/physics_system/bbox.h"
#include "headers/base_engine/renderer/mesh.h"

struct collision_mesh_t
{
  std::vector<glm::vec3> vertices{};
};

/*
  we can make this generic since the criteria is a vec3 anyway
*/
/*
  partial spacial tree since there is a smallest possible subcubesize
  where there can be multiple meshes in one cube
  there can also be the same mesh in mutiple octals
*/
struct partial_spacial_tree_t
{
  struct tree_node_t
  {
    std::vector<collision_mesh_t> meshes{};
    std::array<tree_node_t*, 8> children{};
  };

  std::vector<aabb_t>
  generate(const std::vector<mesh_t>& _meshes)
  {
    static constexpr auto toVec3 = [](const aiVector3D& _vec) { return glm::vec3{_vec.x, _vec.y, _vec.z}; };
    /* find coords and sizes for all covering bboc */
    glm::vec3 min{FLT_MAX, FLT_MAX, FLT_MAX}, max{0.f, 0.f, 0.f};

    f64 average_bbox_size = 0;

    std::vector<aabb_t> out{};

    // std::deque<mesh_t> _meshes{};

    // for (const auto& ite : _meshes_)
    //{
    //   _meshes.push_back(ite);
    // }

    for (const auto& mesh : _meshes)
    {
      average_bbox_size += glm::vec3{toVec3(mesh.bbox.mMax) - toVec3(mesh.bbox.mMin)}.length();

      if (mesh.bbox.mMin.x < min.x)
      {
        min.x = mesh.bbox.mMin.x;
      }
      if (mesh.bbox.mMin.y < min.y)
      {
        min.y = mesh.bbox.mMin.y;
      }
      if (mesh.bbox.mMin.z < min.z)
      {
        min.z = mesh.bbox.mMin.z;
      }

      if (mesh.bbox.mMax.x > max.x)
      {
        max.x = mesh.bbox.mMax.x;
      }
      if (mesh.bbox.mMax.y > max.y)
      {
        max.y = mesh.bbox.mMax.y;
      }
      if (mesh.bbox.mMax.x > max.z)
      {
        max.z = mesh.bbox.mMax.z;
      }
    }

    f32 max_dist = FLT_MIN;

    if ((max.x - min.x) > max_dist)
    {
      max_dist = (max.x - min.x);
    }
    if ((max.y - min.y) > max_dist)
    {
      max_dist = (max.y - min.y);
    }
    if ((max.z - min.z) > max_dist)
    {
      max_dist = (max.z - min.z);
    }

    max = {min.x + max_dist, min.y + max_dist, min.z + max_dist};
    average_bbox_size /= _meshes.size();

    usize curr_depth = 0;
    auto curr_min = min, curr_max = max;

    curr_min = min;
    curr_max = (min + max) / 2.0f;

    std::array<std::vector<collision_mesh_t>, 8> new_octals{};

    // helper for splitting up a cube into octals
    struct sub_octal_t
    {
      glm::vec3 m_min, m_max;

      sub_octal_t(glm::vec3 _min, glm::vec3 _max, usize _index)
      {
        const auto dist = (_max.x - _min.x) / 2.f;

        switch (_index)
        {
        case 0:
          m_min = glm::vec3{_min.x, _min.y, _min.z};
          m_max = glm::vec3{_min.x + dist, _min.y + dist, _min.z + dist};
          break;
        case 1:
          m_min = glm::vec3{_min.x + dist, _min.y, _min.z};
          m_max = glm::vec3{_min.x + dist * 2.f, _min.y + dist, _min.z + dist};
          break;
        case 2:
          m_min = glm::vec3{_min.x, _min.y + dist, _min.z};
          m_max = glm::vec3{_min.x + dist, _min.y + dist * 2.f, _min.z + dist};
          break;
        case 3:
          m_min = glm::vec3{_min.x + dist, _min.y + dist, _min.z};
          m_max = glm::vec3{_min.x + dist * 2.f, _min.y + dist * 2.f, _min.z + dist};
          break;
        case 4:
          m_min = glm::vec3{_min.x, _min.y, _min.z + dist};
          m_max = glm::vec3{_min.x + dist, _min.y + dist, _min.z + dist * 2.f};
          break;
        case 5:
          m_min = glm::vec3{_min.x + dist, _min.y, _min.z + dist};
          m_max = glm::vec3{_min.x + dist * 2.f, _min.y + dist, _min.z + dist * 2.f};
          break;
        case 6:
          m_min = glm::vec3{_min.x, _min.y + dist, _min.z + dist};
          m_max = glm::vec3{_min.x + dist, _min.y + dist * 2.f, _min.z + dist * 2.f};
          break;
        case 7:
          m_min = glm::vec3{_min.x + dist, _min.y + dist, _min.z + dist};
          m_max = glm::vec3{_min.x + dist * 2.f, _min.y + dist * 2.f, _min.z + dist * 2.f};
          break;
        default:
          break;
        }
      }
    };

    // while (true)
    // {

    //   // break at the smallest iteration
    //   if ((curr_max - curr_min).length() <= average_bbox_size)
    //   {
    //     break;
    //   }

    //   ++curr_depth;
    // }

    out.push_back({min, max});

    for (usize i = 0; i < 8; ++i)
    {
      auto funny = sub_octal_t(min, max, i);
      out.push_back({funny.m_min, funny.m_max});
    }

    return out;
  }

  glm::vec3 origin{};
  tree_node_t root;
};
