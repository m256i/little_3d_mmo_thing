#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <base_engine/renderer/mesh.h>
#include <base_engine/renderer/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "../../../common.h"

class render_model_t
{
public:
  render_model_t() = default;

  // model data
  std::vector<mesh_t::texture_t> textures_loaded; // stores all the textures loaded so far, optimization to make sure
                                                  // textures aren't loaded more than once.
  std::vector<mesh_t> meshes;
  std::string_view directory;
  bool gamme_corretion;

  // constructor, expects a filepath to a 3D model.
  explicit render_model_t(std::string const &path, bool gamma = false) : gamme_corretion(gamma) { load_render_model(path); }

  // draws the model, and thus all its meshes
  void
  draw(const basic_shader_t &_shader, usize instance_count = 1) const
  {
    for (auto &mesh : meshes)
    {
      mesh.draw(_shader, instance_count);
    }
  }

  // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void load_render_model(std::string_view path);
  // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this
  // process on its children nodes (if any).
  void process_node(aiNode *node, const aiScene *scene);

  mesh_t process_mesh(aiMesh *mesh, const aiScene *scene);
  // checks all material textures of a given type and loads the textures if they're not loaded yet.
  // the required info is returned as a Texture struct.
  std::vector<mesh_t::texture_t> load_mat_textures(aiMaterial *mat, aiTextureType type, std::string_view typeName);
};
