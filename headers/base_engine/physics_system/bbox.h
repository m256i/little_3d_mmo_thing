#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <array>

#include <assimp/aabb.h>

#include "../../common.h"
#include "assimp/vector3.h"
#include "glm/ext/vector_float3.hpp"

/*
TODO:
  - make octree with AABBs then do collision based on direct vertices
*/

struct aabb_t
{
  glm::vec3 min, max;

  void
  operator=(const aiAABB& vec)
  {
    min.x = vec.mMin.x;
    min.y = vec.mMin.y;
    min.z = vec.mMin.z;
    max.x = vec.mMax.x;
    max.y = vec.mMax.y;
    max.z = vec.mMax.z;
  }

  glm::vec3
  get_center()
  {
    return ((min + max) / 2.f);
  }

  glm::vec3
  get_scaling()
  {
    return ((max - min) / 2.f);
  }
};

struct debug_bbox_t
{
  debug_bbox_t() = default;

  struct vert_t
  {
    glm::vec3 origin;

    void
    operator=(const aiVector3D& vec)
    {
      origin.x = vec.x;
      origin.y = vec.y;
      origin.z = vec.z;
    }

    void
    operator=(const glm::vec3& vec)
    {
      origin = vec;
    }

    glm::vec3
    operator+(const glm::vec3& vec)
    {
      return origin + vec;
    }

    glm::vec3
    operator*(const glm::vec3& vec)
    {
      return origin * vec;
    }
  };

  std::array<vert_t, 8> vertices{
      glm::vec3{-1, -1, 1}, // 0 bottom left
      glm::vec3{1, -1, 1},  // 1
      glm::vec3{-1, 1, 1},  // 2
      glm::vec3{1, 1, 1},   // 3

      glm::vec3{-1, -1, -1}, // 4
      glm::vec3{1, -1, -1},  // 5
      glm::vec3{-1, 1, -1},  // 6
      glm::vec3{1, 1, -1}    // 7 top right
  };

  std::array<u32, 36> indices = {
      0, 1, 3, 2, // front face
      4, 5, 7, 6, // back face
      0, 4, 6, 2, // left face
      1, 5, 7, 3, // right face
      2, 6, 7, 3, // top face
      0, 4, 5, 1  // bottom face
  };
  u32 vbo, vao, ebo;

  void
  load_from_mesh(const aiAABB& _bbox)
  {
    glm::vec3 scalingFactors = glm::vec3{_bbox.mMax.x - _bbox.mMin.x, _bbox.mMax.y - _bbox.mMin.y, _bbox.mMax.z - _bbox.mMin.z} / 2.0f;
    glm::vec3 center         = glm::vec3{(_bbox.mMax.x + _bbox.mMin.x) / 2.f, (_bbox.mMax.y + _bbox.mMin.y) / 2.f, (_bbox.mMax.z + _bbox.mMin.z) / 2.f};

    for (int i = 0; i < 8; i++)
    {
      vertices[i] = center + (vertices[i].origin * scalingFactors);
    }
  }

  void
  bind()
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vert_t), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert_t), (void*)0);
  }

  void
  draw()
  {
    glBindVertexArray(vao);
    glDrawElements(GL_QUADS, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
};