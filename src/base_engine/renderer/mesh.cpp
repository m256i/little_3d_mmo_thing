#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <base_engine/renderer/mesh.h>
#include <include/assimp/aabb.h>
#include <base_engine/renderer/shader.h>
#include <glad/glad.h>

#include <string_view>
#include <vector>
#include <array>
#include <string>

#include "../../common.h"

#ifdef min
#undef min
#else
#ifdef max
#undef max
#endif
#endif

void
mesh_t::draw(const basic_shader_t &shader, usize instance_count) const
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

  if (instance_count > 1) [[unlikely]]
  {
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, instance_count);
  }
  else
  {
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
  }

  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void
mesh_t::bind_mesh_data()
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
