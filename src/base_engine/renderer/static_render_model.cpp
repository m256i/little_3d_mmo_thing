#include <base_engine/renderer/static_render_model.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <glm/glm.hpp>

#include <string>

#include <base_engine/renderer/core/tex_atlas.h>
#include <stb_image.h>

struct temporary_mesh
{
  struct vert
  {
    glm::vec3 pos, normal;
    glm::vec2 texture_coords;
  };

  std::string texture_file_path;
  std::vector<vert> vertices;
  std::vector<u32> indices;
};

inline temporary_mesh
process_mesh(aiMesh *mesh, const aiScene *scene)
{
  temporary_mesh out;

  for (usize i = 0; i < mesh->mNumVertices; i++)
  {
    temporary_mesh::vert vertex;

    vertex.pos.x = mesh->mVertices[i].x;
    vertex.pos.y = mesh->mVertices[i].y;
    vertex.pos.z = mesh->mVertices[i].z;

    if (mesh->HasNormals())
    {
      vertex.normal.x = mesh->mNormals[i].x;
      vertex.normal.y = mesh->mNormals[i].y;
      vertex.normal.z = mesh->mNormals[i].z;
    }

    if (mesh->mTextureCoords[0])
    {
      vertex.texture_coords.x = mesh->mTextureCoords[0][i].x;
      vertex.texture_coords.y = mesh->mTextureCoords[0][i].y;

      // // tangent
      // vertex.tangents.x = mesh->mTangents[i].x;
      // vertex.tangents.y = mesh->mTangents[i].y;
      // vertex.tangents.z = mesh->mTangents[i].z;

      // // bitangent
      // vertex.bitangents.x = mesh->mBitangents[i].x;
      // vertex.bitangents.y = mesh->mBitangents[i].y;
      // vertex.bitangents.z = mesh->mBitangents[i].z;
    }
    else
    {
      vertex.texture_coords = glm::vec2(0.0f, 0.0f);
    }

    out.vertices.push_back(vertex);
  }

  for (usize i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    for (usize j = 0; j < face.mNumIndices; j++)
    {
      out.indices.push_back(face.mIndices[j]);
    }
  }

  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  /*
  FIXME: im lazy we also need other types of textures like specular and normal
  */
  for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
  {
    aiString str;
    material->GetTexture(aiTextureType_DIFFUSE, i, &str);
    printf("found used texture: %s\n", str.C_Str());
    out.texture_file_path = std::string(str.C_Str());
  }

  return out;
}

inline u0
process_node(aiNode *node, const aiScene *scene, std::vector<temporary_mesh> &meshes)
{
  for (usize i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(process_mesh(mesh, scene));
  }

  for (usize i = 0; i < node->mNumChildren; i++)
  {
    process_node(node->mChildren[i], scene, meshes);
  }
}

namespace renderer
{
u0
static_render_model::load_from_file(std::string_view _path)
{
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(_path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
                                          aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GenBoundingBoxes);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    LOG(INFO) << "[model] : error: '" << importer.GetErrorString() << "'";
    return;
  }

  const std::string directory = std::string{_path.substr(0, _path.find_last_of('/'))};

  std::vector<temporary_mesh> meshes;
  process_node(scene->mRootNode, scene, meshes);

  // assert(false);

  core::textures::texture_atlas atlas;

  /* load all textures and pack 'em */
  for (const auto &mesh : meshes)
  {
    auto &tex = mesh.texture_file_path;

    i32 tex_sx, tex_sy, num_channels;
    u8 *texture_buf = stbi_load((directory + "/" + tex).c_str(), &tex_sx, &tex_sy, &num_channels, STBI_default);

    if (!texture_buf)
    {
      LOG(INFO) << "failed to load texture: " << (directory + "/" + tex);
      assert(false);
    }

    atlas.add_texture(fnv1a::hash(tex.c_str()), texture_buf, tex_sx, tex_sy, num_channels);

    LOG(INFO) << "num channels: " << num_channels;
  }

  // assert(false);

  pipeline.add_uniform_callack("model", [&]() { return model; });
  pipeline.add_uniform_callack("projection", [&]() { return projection; });
  pipeline.add_uniform_callack("view", [&]() { return view; });

  auto atlas_pointer = atlas.generate();

  pipeline.load_texture_from_memory("texture_diffuse1", atlas_pointer, atlas.atlas_width, atlas.atlas_height, atlas.num_channels);
  pipeline.initialize();

  u32 current_index_offset{};
  for (auto &mesh : meshes)
  {
    for (auto &vert : mesh.vertices)
    {
      auto uv_map = atlas.get_uv_map(fnv1a::hash(mesh.texture_file_path.c_str()), vert.texture_coords.x, vert.texture_coords.y);
      /* add the atlas index to the UV here */
      vert.texture_coords.x = uv_map.first;
      vert.texture_coords.y = uv_map.second;

      pipeline.push_back_vertex<temporary_mesh::vert>(vert);
    }
    for (const u32 index : mesh.indices)
    {
      pipeline.push_back_index(index + current_index_offset);
    }
    current_index_offset += mesh.indices.size();
  }

  atlas.destroy();

  pipeline.setup_drawbuffer();
}
} // namespace renderer