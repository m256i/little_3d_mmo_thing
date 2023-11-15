#pragma once

#include <cmath>
#include <exception>
#include <glm/glm.hpp>
#include <vector>

#include <base_engine/renderer/mesh.h>
#include "../../common.h"

struct convex_hull_t
{
  convex_hull_t() = default;
  glm::vec3 center{};
  std::vector<glm::vec3> points{};
  std::vector<usize> indices{};

  std::vector<std::vector<glm::vec3>> simplified_chulls{};

  u0
  load(const mesh_t& _mesh);

  u0
  from_mesh(const mesh_t& _mesh);

  // distance based reduce
  std::vector<glm::vec3>
  reduce(const std::vector<glm::vec3>& _vertices, double _min_distance = 0.f);
 
  bool
  to_submeshes();
  // this should be done to collision mesh not chull
  bool
  simplify();
};
