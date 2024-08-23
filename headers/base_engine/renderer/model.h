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
#include <vector>

#include <common.h>
#include "core/image_texture.h"
#include "core/lod.h"
#include "render_primitives/bbox.h"

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

  primitives::aabb mesh_aabb;

  f32 model_scale_factor = 1.f;

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

class lod_render_model_t
{
public:
  lod_render_model_t() = default;

  // since every texture has its own lod level we only need to load
  // them once for every model
  std::vector<renderer::image_tex_lod> textures_loaded; // stores all the textures loaded so far
  struct lod_model_instance_data
  {
    std::vector<lod_mesh_t> meshes;
  };

  std::array<lod_model_instance_data, (usize)lod::detail_level::lod_detail_enum_size> lod_meshes{};

  std::string_view directory;

  bool gamme_corretion;

  f32 model_scale_factor;

  // constructor, expects a filepath to a 3D model.
  explicit lod_render_model_t(std::string const &path, bool gamma = false) : gamme_corretion(gamma) { load_render_model(path); }

  // draws the model, and thus all its meshes
  void
  draw(const basic_shader_t &_shader, lod::detail_level lod_level, usize instance_count = 1) const
  {
    assert((usize)lod_level < lod_meshes.size());

    auto &lod_mesh = lod_meshes[(usize)lod_level];

    for (auto &mesh : lod_mesh.meshes)
    {
      mesh.draw(_shader, lod_level, instance_count);
    }
  }

  void load_render_model(std::string_view path);

  // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void load_render_model_internal(std::string_view path, lod::detail_level lod_level);
  // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this
  // process on lod_mesh_tits children nodes (if any).
  void process_node(aiNode *node, const aiScene *scene, lod::detail_level lod_level);

  lod_mesh_t process_mesh(aiMesh *mesh, const aiScene *scene, lod::detail_level lod_level);
  // checks all material textures of a given type and loads the textures if they're not loaded yet.
  // the required info is returned as a Texture struct.
  std::vector<renderer::image_tex_lod> load_mat_textures(aiMaterial *mat, aiTextureType type, std::string_view typeName);
};
