#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <base_engine/renderer/shader.h>
#include <assimp/aabb.h>

#include <string_view>
#include <vector>
#include <array>
#include <string>

#include <common.h>
#include "core/image_texture.h"

struct mesh_t
{
  static constexpr usize max_bone_deps = 4;
  struct vertex_t
  {
    // position
    glm::vec3 position;
    // normal
    glm::vec3 normal;
    // texCoords
    glm::vec2 tex_coords;
    // tangent
    glm::vec3 tangent;
    // bitangent
    glm::vec3 bitangent;
    // bone indexes which will influence this vertex
    i32 bone_ids[max_bone_deps];
    // weights from each bone
    f32 weights[max_bone_deps];
  };

  struct texture_t
  {
    u32 id, width, height;
    std::string type;
    std::string path;
  };

  // mesh Data
  std::vector<vertex_t> vertices;
  std::vector<u32> indices;
  std::vector<texture_t> textures;
  aiAABB bbox;
  u32 VAO, VBO, EBO;

  // constructor
  mesh_t(std::vector<vertex_t> vertices, std::vector<u32> indices, std::vector<texture_t> textures, const aiAABB &_bbox)
  {
    this->vertices = vertices;
    this->indices  = indices;
    this->textures = textures;
    bbox           = _bbox;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    bind_mesh_data();
  }

  // render the mesh
  void draw(const basic_shader_t &shader, usize instance_count = 1) const;
  // render data

  // initializes all the buffer objects/arrays
  void bind_mesh_data();
};

struct lod_mesh_t
{
  static constexpr usize max_bone_deps = 4;
  struct vertex_t
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    i32 bone_ids[max_bone_deps];
    f32 weights[max_bone_deps];
  };

  std::vector<vertex_t> vertices;
  std::vector<u32> indices;
  std::vector<renderer::image_tex_lod> textures;
  aiAABB bbox;
  u32 VAO, VBO, EBO;

  // constructor
  lod_mesh_t(std::vector<vertex_t> vertices, std::vector<u32> indices, std::vector<renderer::image_tex_lod> textures, const aiAABB &_bbox)
  {
    this->vertices = vertices;
    this->indices  = indices;
    this->textures = textures;
    bbox           = _bbox;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    bind_mesh_data();
  }

  // render the mesh
  void draw(const basic_shader_t &shader, lod::detail_level lod_level, usize instance_count = 1) const;
  // render data

  // initializes all the buffer objects/arrays
  void bind_mesh_data();
};
