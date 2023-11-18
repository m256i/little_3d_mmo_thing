#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include <glm/vec3.hpp>
#include <deque>
#include <algorithm>

#include <base_engine/renderer/mesh.h>
#include <base_engine/physics_system/bbox.h>

#include "../../common.h"

/*
TODO: generate convex hull for every mesh and put that into the octree
split mesh into faces with normals for collision checking
*/

struct collision_mesh_t
{
  std::vector<mesh_t::vertex_t> vertices{};
};

/*
  we can make this generic since the criteria is a vec3 anyway
*/
/*
  partial spacial tree since there is a smallest possible subcubesize
  where there can be multiple meshes in one cube
  there can also be the same mesh in mutiple octals
*/
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

struct tree_node_t
{
  aabb_t bbox{};
  std::vector<collision_mesh_t> meshes{};
  std::array<tree_node_t*, 8> children{};
};

struct partial_spacial_tree_t
{
  bool generate(std::vector<mesh_t>& _meshes);

  std::vector<collision_mesh_t> find(glm::vec3 _position);

  glm::vec3 origin{};
  tree_node_t root;
};
