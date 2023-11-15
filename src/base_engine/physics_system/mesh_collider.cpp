#include <array>
#include <exception>
#include <float.h>
#include <limits>
#include <sal.h>
#include <unordered_map>
#include <vector>
#include <glm/vec3.hpp>
#include <deque>
#include <algorithm>

#include <base_engine/physics_system/mesh_collider.h>
#include <base_engine/physics_system/bbox.h>
#include <base_engine/physics_system/chull.h>
#include <base_engine/renderer/mesh.h>

inline auto
bbox_bbox_collision_center(const aabb_t& _a, const aabb_t& _b) -> glm::vec3
{
  float overlap_x = mmax(0.0f, mmin(_a.max.x, _b.max.x) - mmax(_a.min.x, _b.min.x));
  float overlap_y = mmax(0.0f, mmin(_a.max.y, _b.max.y) - mmax(_a.min.y, _b.min.y));
  float overlap_z = mmax(0.0f, mmin(_a.max.z, _b.max.z) - mmax(_a.min.z, _b.min.z));

  // Calculate the center of the overlap area
  glm::vec3 overlap_center;
  overlap_center.x = mmax(_a.min.x, _b.min.x) + overlap_x / 2.0f;
  overlap_center.y = mmax(_a.min.y, _b.min.y) + overlap_y / 2.0f;
  overlap_center.z = mmax(_a.min.z, _b.min.z) + overlap_z / 2.0f;

  return overlap_center;
}

inline auto
chull_bbox_collision_center(const convex_hull_t& _hull, const aabb_t& _b) -> glm::vec3
{
  static constexpr auto point_in_bbox = [](const glm::vec3& _point, const aabb_t& _bb)
  {
    return (_point.x < _bb.max.x && _point.y < _bb.max.y && _point.z < _bb.max.z && _point.x > _bb.min.x &&
            _point.y > _bb.min.y && _point.z > _bb.min.z);
  };

  std::vector<glm::vec3> points_inside{};
  glm::vec3 center{};

  for (const auto& point : _hull.points)
  {
    if (point_in_bbox(point, _b))
    {
      points_inside.push_back(point);
    }
  }

  for (const auto& point : points_inside)
  {
    center += point;
  }

  center /= points_inside.size();
  return center;
}

inline auto
bbox_overlaps_bbox(const aabb_t& _a, const aabb_t& _b) -> bool
{
  if (_a.max.x < _b.min.x || _a.min.x > _b.max.x) return false; // No overlap in the X-axis
  if (_a.max.y < _b.min.y || _a.min.y > _b.max.y) return false; // No overlap in the Y-axis
  if (_a.max.z < _b.min.z || _a.min.z > _b.max.z) return false; // No overlap in the Z-axis

  return true;
}

// TODO FIX
inline auto
chull_overlaps_bbox(const convex_hull_t& _hull, const aabb_t& _bb) -> bool
{
  for (const auto& point : _hull.points)
  {
    if (point.x > _bb.max.x || point.y > _bb.max.y || point.z > _bb.max.z || point.x < _bb.max.x ||
        point.y < _bb.max.y || point.z < _bb.max.z)

      return false;
  }
  return true;
}

inline auto
point_inside_bbox(const aabb_t& _box, const glm::vec3& _point) -> bool
{ // bruh
  return (_point.x >= _box.min.x && _point.y >= _box.min.y && _point.z >= _box.min.z) &&
         (_point.x <= _box.max.x && _point.y <= _box.max.y && _point.z <= _box.max.z);
}

// we can assume ccw orientation so we can use points sequential to each other
inline auto
point_inside_chull(const glm::vec3& _point, const convex_hull_t _chull)
{
  static constexpr auto get_side =
      [](const glm::vec3& point, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
  {
    glm::vec3 v_point = point - p1;
    glm::vec3 normal  = glm::cross(p2 - p1, p3 - p1);
    normal            = glm::normalize(normal);

    f32 dot_point = glm::dot(normal, v_point);
    bool on_side  = dot_point > 0;
    return on_side;
  };

  for (usize i = 0; i != _chull.indices.size() - 2; i += 3)
  {
    auto& p1 = _chull.points[_chull.indices[i + 0]];
    auto& p2 = _chull.points[_chull.indices[i + 1]];
    auto& p3 = _chull.points[_chull.indices[i + 2]];

    auto center_side = get_side(_chull.center, p1, p2, p3);

    if (get_side(_point, p1, p2, p3) != center_side)
    {
      return false;
    }
  }
  return true;
}

inline bool
simplify(const std::vector<mesh_t>& _meshes, std::vector<mesh_t>& out_meshes)
{
}

inline bool
recursive_subdivide(tree_node_t& _current_node, const std::vector<mesh_t>& _meshes,
                    std::vector<mesh_t*>& _inside_meshes, std::string _recursion_string)
{
  if (_meshes.empty() || _inside_meshes.empty())
  {
    return true;
  }

  std::vector<mesh_t*> meshes_inside{};

  for (auto pmesh : _inside_meshes)
  {
    if (pmesh == nullptr)
    {
      continue;
    }

    auto& mesh = *pmesh;
    convex_hull_t ch{};
    ch.from_mesh(mesh);

    if (point_inside_bbox(_current_node.bbox, ch.center))
    {
      meshes_inside.push_back(pmesh);
    }
    else if (chull_overlaps_bbox(ch, _current_node.bbox))
    {
      // TODO potentioannly unneeded
      // if (point_inside_bbox(_current_node.bbox, chull_bbox_collision_center(ch, _current_node.bbox)))
      // {
      meshes_inside.push_back(pmesh);
      // }
    }
  }

  for (const auto& ite : meshes_inside)
  {
    auto i = std::find(_inside_meshes.begin(), _inside_meshes.end(), ite);
    if (i != _inside_meshes.end())
    {
      _inside_meshes.erase(i);
    }
  }

  printf("%sfound %llu meshes inside current box!\n", _recursion_string.c_str(), meshes_inside.size());
  LOG(INFO) << _recursion_string.c_str() << "found " << meshes_inside.size() << "meshes inside curernt box!";

  // no meshes inside of this octal no need to parse further
  /* false here indicates that the _current_node node can be deleted since there are no meshes inside */
  if (meshes_inside.empty())
  {
    return false;
  }

  usize unique_counter = 0;
  {
    auto hash  = [](const glm::vec3& n) { return (n.x * 3 + n.y * 5 + n.z * 17); };
    auto equal = [](const glm::vec3& l, const glm::vec3& r) { return l == r; };

    std::unordered_map<glm::vec3, int, decltype(hash), decltype(equal)> centers{};
    for (const auto& ite : meshes_inside)
    {
      convex_hull_t ch{};
      ch.from_mesh(*ite);
      centers[ch.center] = 1;
    }

    unique_counter = centers.size();
  }

  printf("%sfound %llu unique meshes inside current box!\n", _recursion_string.c_str(), meshes_inside.size());
  LOG(INFO) << _recursion_string.c_str() << "found " << meshes_inside.size() << " unique meshes inside current box!";

  // we want exactly 2 or less e unique meshes per cube
  if (unique_counter <= 2)
  {
    printf("%sfound 3 or less meshes inside current box, returning!\n", _recursion_string.c_str());
    LOG(INFO) << _recursion_string.c_str() << "found 3 or less meshes inside current box, returning!";
    for (const auto& ite : meshes_inside)
    {
      _current_node.meshes.push_back(collision_mesh_t{ite->vertices});
    }
    return true;
  }

  for (usize i = 0; i != 8; ++i)
  {
    printf("%soctal index %llu created!\n", _recursion_string.c_str(), i);
    LOG(INFO) << _recursion_string.c_str() << "octal index " << i << " created!";

    auto new_octal                  = sub_octal_t(_current_node.bbox.min, _current_node.bbox.max, i);
    _current_node.children[i]       = new tree_node_t;
    _current_node.children[i]->bbox = aabb_t(new_octal.m_min, new_octal.m_max);

    printf("%smeshes inside size pre recursion: %llu\n", _recursion_string.c_str(), meshes_inside.size());
    LOG(INFO) << _recursion_string << "meshes inside size pre recursion: " << meshes_inside.size();

    if (recursive_subdivide(*_current_node.children[i], _meshes, meshes_inside, _recursion_string + "|  ") == false)
    {
      printf("%smeshes inside size: %llu after call\n", _recursion_string.c_str(), meshes_inside.size());
      LOG(INFO) << _recursion_string << "meshes inside size: " << meshes_inside.size();
      printf("%sno more meshes found deleting branch\n", _recursion_string.c_str());
      LOG(INFO) << _recursion_string << "no more meshes found deleting branch";
      delete _current_node.children[i];
      _current_node.children[i] = nullptr;
    }

    if (meshes_inside.empty())
    {
      printf("%srecursion got rid of all suubmeshes returning!\n", _recursion_string.c_str());
      LOG(INFO) << _recursion_string << "recursion got rid of all suubmeshes returning!";
      return true;
    }
  }

  return false;
}

inline std::vector<tree_node_t*>
find_recursive(const tree_node_t& _node)
{
}

bool
partial_spacial_tree_t::generate(std::vector<mesh_t>& _meshes)
{
  /* find coords and sizes for all covering bboc */
  glm::vec3 min{FLT_MAX, FLT_MAX, FLT_MAX}, max{0.f, 0.f, 0.f};

  std::vector<aabb_t> out{};

  for (const auto& mesh : _meshes)
  {
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

  root.bbox = {min, max};

  std::vector<mesh_t*> inside_meshes{};
  for (auto& ite : _meshes)
  {
    inside_meshes.push_back(&ite);
  }

  auto result = recursive_subdivide(root, _meshes, inside_meshes, " ");
  return result;
}

std::vector<collision_mesh_t>
partial_spacial_tree_t::find(glm::vec3 _position)
{
  tree_node_t *final_node = nullptr, *current_node = &root;

  usize already_seen = 0;

  while (final_node == nullptr)
  {
    already_seen = 0;
    for (const auto ite : current_node->children)
    {
      if (ite == nullptr)
      {
        already_seen++;
        continue;
      }
      if (point_inside_bbox(ite->bbox, _position))
      {
        current_node = ite;
        break;
      }
      already_seen++;
    }

    if (already_seen == 8)
    {
      final_node = current_node;
    }
  }

  return final_node->meshes;
}
