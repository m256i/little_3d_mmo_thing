#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/aabb.h"
#include "shader.h"

#include <string_view>
#include <vector>
#include <array>
#include <string>
#include "../../common.h"

#ifdef min(x, y)
#undef min(x, y)
#else
#ifdef max(x, y)
#undef max(x, y)
#endif
#endif

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
    u32 id;
    std::string type;
    std::string path;
  };

  // mesh Data
  std::vector<vertex_t> vertices;
  std::vector<u32> indices;
  std::vector<texture_t> textures;
  aiAABB bbox;
  u32 VAO;

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
  void
  draw(const basic_shader_t &shader) const
  {
    // bind appropriate textures
    u32 diffuseNr  = 1;
    u32 specularNr = 1;
    u32 normalNr   = 1;
    u32 heightNr   = 1;
    for (usize i = 0; i < textures.size(); i++)
    {
      glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
      // retrieve texture number (the N in diffuse_textureN)
      std::string number;
      std::string name = textures[i].type;
      if (name == "texture_diffuse")
      {
        number = std::to_string(diffuseNr++);
      }
      else if (name == "texture_specular")
      {
        number = std::to_string(specularNr++); // transfer unsigned int to string
      }
      else if (name == "texture_normal")
      {
        number = std::to_string(normalNr++); // transfer unsigned int to string
      }
      else if (name == "texture_height")
      {
        number = std::to_string(heightNr++); // transfer unsigned int to string
      }

      // now set the sampler to the correct texture unit
      glUniform1i(glGetUniformLocation(shader.id, (name + number).c_str()), i);
      // and finally bind the texture
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
  }

  // render data
  u32 VBO, EBO;

  // initializes all the buffer objects/arrays
  void
  bind_mesh_data()
  {
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array
    // which again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_t), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, position));
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, tex_coords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(vertex_t), (void *)offsetof(vertex_t, bone_ids));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, weights));

    glBindVertexArray(0);
  }
};
