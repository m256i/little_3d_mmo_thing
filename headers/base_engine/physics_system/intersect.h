#pragma once

#include <base_engine/physics_system/bbox.h>
#include <base_engine/physics_system/base_shapes.h>
#include <base_engine/physics_system/chull.h>

#include <glm/glm.hpp>

inline bool
isect_tri_aabb_sat(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 aabbExtents, glm::vec3 axis)
{
  float p0 = glm::dot(v0, axis);
  float p1 = glm::dot(v1, axis);
  float p2 = glm::dot(v2, axis);

  float r = aabbExtents.x * std::fabsf(glm::dot(glm::vec3(1, 0, 0), axis)) +
            aabbExtents.y * std::fabsf(glm::dot(glm::vec3(0, 1, 0), axis)) + aabbExtents.z * std::fabsf(glm::dot(glm::vec3(0, 0, 1), axis));

  float maxP = mmax(p0, mmax(p1, p2));
  float minP = mmin(p0, mmin(p1, p2));

  return !(mmax(-maxP, minP) > r);
}

inline bool
isect_tri_aabb(triangle_t tri, aabb_t aabb)
{
  tri.a -= aabb.get_center();
  tri.b -= aabb.get_center();
  tri.c -= aabb.get_center();

  glm::vec3 ab = glm::normalize(tri.b - tri.a);
  glm::vec3 bc = glm::normalize(tri.c - tri.b);
  glm::vec3 ca = glm::normalize(tri.a - tri.c);

  // Cross ab, bc, and ca with (1, 0, 0)
  glm::vec3 a00 = glm::vec3(0.0, -ab.z, ab.y);
  glm::vec3 a01 = glm::vec3(0.0, -bc.z, bc.y);
  glm::vec3 a02 = glm::vec3(0.0, -ca.z, ca.y);

  // Cross ab, bc, and ca with (0, 1, 0)
  glm::vec3 a10 = glm::vec3(ab.z, 0.0, -ab.x);
  glm::vec3 a11 = glm::vec3(bc.z, 0.0, -bc.x);
  glm::vec3 a12 = glm::vec3(ca.z, 0.0, -ca.x);

  // Cross ab, bc, and ca with (0, 0, 1)
  glm::vec3 a20 = glm::vec3(-ab.y, ab.x, 0.0);
  glm::vec3 a21 = glm::vec3(-bc.y, bc.x, 0.0);
  glm::vec3 a22 = glm::vec3(-ca.y, ca.x, 0.0);

  if (!isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a00) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a01) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a02) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a10) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a11) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a12) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a20) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a21) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), a22) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), glm::vec3(1, 0, 0)) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), glm::vec3(0, 1, 0)) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), glm::vec3(0, 0, 1)) ||
      !isect_tri_aabb_sat(tri.a, tri.b, tri.c, aabb.get_extents(), glm::cross(ab, bc)))
  {
    return false;
  }

  return true;
}

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
    return (_point.x < _bb.max.x && _point.y < _bb.max.y && _point.z < _bb.max.z && _point.x > _bb.min.x && _point.y > _bb.min.y &&
            _point.z > _bb.min.z);
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
  if (_a.max.x < _b.min.x || _a.min.x > _b.max.x) return false;
  if (_a.max.y < _b.min.y || _a.min.y > _b.max.y) return false;
  if (_a.max.z < _b.min.z || _a.min.z > _b.max.z) return false;

  return true;
}

inline auto
chull_overlaps_bbox(const convex_hull_t& _hull, const aabb_t& _bb) -> bool
{
  for (const auto& point : _hull.points)
  {
    if (point.x > _bb.max.x || point.y > _bb.max.y || point.z > _bb.max.z || point.x < _bb.max.x || point.y < _bb.max.y ||
        point.z < _bb.max.z)

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

// we can assume ccw orientation so we can use points next to each other
inline auto
point_inside_chull(const glm::vec3& _point, const convex_hull_t _chull)
{
  static constexpr auto get_side = [](const glm::vec3& point, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
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