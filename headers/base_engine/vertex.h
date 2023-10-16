#pragma once

#include "../../common.h"
#include <vector>

struct vertex_t
{
  f32 x, y, z;
};

struct vertex_buffer_t
{
  std::vector<vertex_t> vertecies{};

  bool
  bind()
  {
  }
};
