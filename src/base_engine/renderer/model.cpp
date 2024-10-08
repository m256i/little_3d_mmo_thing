#include <base_engine/renderer/model.h>

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

#include <logging/easylogging++.h>

#include <common.h>
#include "base_engine/renderer/core/image_texture.h"
#include "base_engine/renderer/core/lod.h"

#undef min(x, y)
#undef max(x, y)

inline u32
load_texture_from_file(std::string_view path, std::string_view directory, u32 &_width, u32 &_height)
{
  std::string filename = std::string(path);
  filename             = std::string(directory) + '/' + filename;

  u32 texture_id;
  glGenTextures(1, &texture_id);

  i32 width, height, num_channels;
  u8 *data = stbi_load(filename.c_str(), &width, &height, &num_channels, 0);

  _width  = width;
  _height = height;

  if (data)
  {
    u32 format{0};
    switch (num_channels)
    {
    case 1:
    {
      format = (u32)GL_RED;
      break;
    }
    case 3:
    {
      format = (u32)GL_RGB;
      break;
    }
    case 4:
    {
      format = (u32)GL_RGBA;

      for (int i = 0; i < width * height * 4; i += 4)
      {
        float alpha = data[i + 3] / 255.0f;
        data[i]     = static_cast<unsigned char>(data[i] * alpha);     // R
        data[i + 1] = static_cast<unsigned char>(data[i + 1] * alpha); // G
        data[i + 2] = static_cast<unsigned char>(data[i + 2] * alpha); // B
                                                                       // data[i + 3] is already the alpha channel
      }

      break;
    }
    default:
    {
    }
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    LOG(INFO) << "[model] : texture failed to load: " << path;
    stbi_image_free(data);
  }

  return texture_id;
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void
render_model_t::load_render_model(std::string_view path)
{
  // read file via ASSIMP
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
                                         aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GenBoundingBoxes);
  // check for errors
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
  {
    LOG(INFO) << "[model] : error: '" << importer.GetErrorString() << "'";
    return;
  }
  // retrieve the directory path of the filepath
  directory = path.substr(0, path.find_last_of('/'));

  // process ASSIMP's root node recursively
  process_node(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this
// process on its children nodes (if any).
void
render_model_t::process_node(aiNode *node, const aiScene *scene)
{
  // process each mesh located at the current node
  for (usize i = 0; i < node->mNumMeshes; i++)
  {
    // the node object only contains indices to index the actual objects in the scene.
    // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(process_mesh(mesh, scene));
  }
  // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for (usize i = 0; i < node->mNumChildren; i++)
  {
    process_node(node->mChildren[i], scene);
  }
}

mesh_t
render_model_t::process_mesh(aiMesh *mesh, const aiScene *scene)
{
  mesh_aabb.expand_to_bbox(mesh->mAABB.mMin, mesh->mAABB.mMax);

  // data to fill
  std::vector<mesh_t::vertex_t> vertices;
  std::vector<unsigned int> indices;
  std::vector<mesh_t::texture_t> textures;

  aiAABB bbox = mesh->mAABB;

  // walk through each of the mesh's vertices
  for (usize i = 0; i < mesh->mNumVertices; i++)
  {
    mesh_t::vertex_t vertex;
    glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly
                      // convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
    // positions
    vector.x        = mesh->mVertices[i].x;
    vector.y        = mesh->mVertices[i].y;
    vector.z        = mesh->mVertices[i].z;
    vertex.position = vector;
    // normals
    if (mesh->HasNormals())
    {
      vector.x      = mesh->mNormals[i].x;
      vector.y      = mesh->mNormals[i].y;
      vector.z      = mesh->mNormals[i].z;
      vertex.normal = vector;
    }
    // texture coordinates
    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
      // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
      vec.x             = mesh->mTextureCoords[0][i].x;
      vec.y             = mesh->mTextureCoords[0][i].y;
      vertex.tex_coords = vec;
      // tangent
      vector.x       = mesh->mTangents[i].x;
      vector.y       = mesh->mTangents[i].y;
      vector.z       = mesh->mTangents[i].z;
      vertex.tangent = vector;
      // bitangent
      vector.x         = mesh->mBitangents[i].x;
      vector.y         = mesh->mBitangents[i].y;
      vector.z         = mesh->mBitangents[i].z;
      vertex.bitangent = vector;
    }
    else
    {
      vertex.tex_coords = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }

  // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex
  // indices.
  for (usize i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    // retrieve all indices of the face and store them in the indices vector
    for (usize j = 0; j < face.mNumIndices; j++)
    {
      indices.push_back(face.mIndices[j]);
    }
  }
  // process materials
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
  // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
  // Same applies to other texture as the following list summarizes:
  // diffuse: texture_diffuseN
  // specular: texture_specularN
  // normal: texture_normalN

  // 1. diffuse maps
  std::vector<mesh_t::texture_t> diffuseMaps = load_mat_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  // 2. specular maps
  std::vector<mesh_t::texture_t> specularMaps = load_mat_textures(material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  // 3. normal maps
  std::vector<mesh_t::texture_t> normalMaps = load_mat_textures(material, aiTextureType_NORMALS, "texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
  // 4. height maps
  std::vector<mesh_t::texture_t> heightMaps = load_mat_textures(material, aiTextureType_HEIGHT, "texture_height");
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  // return a mesh object created from the extracted mesh data
  return mesh_t(vertices, indices, textures, bbox);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<mesh_t::texture_t>
render_model_t::load_mat_textures(aiMaterial *mat, aiTextureType type, std::string_view typeName)
{
  std::vector<mesh_t::texture_t> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    bool skip = false;
    for (unsigned int j = 0; j < textures_loaded.size(); j++)
    {
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
      {
        textures.push_back(textures_loaded[j]);
        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
        break;
      }
    }
    if (!skip)
    { // if texture hasn't been loaded already, load it
      mesh_t::texture_t texture;
      texture.id   = load_texture_from_file({str.C_Str()}, this->directory, texture.width, texture.height);
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't
                                          // unnecessary load duplicate textures.
    }
  }
  return textures;
}

#include <meshoptimizer.h>

void
lod_render_model_t::load_render_model(std::string_view path)
{
  for (usize i = 0; i < (usize)lod::detail_level::lod_detail_enum_size; ++i)
  {
    if (i == 0)
    {
      lod_render_model_t::load_render_model_internal(path, (lod::detail_level)i);
      continue;
    }
    lod_render_model_t::load_render_model_internal(path, (lod::detail_level)i);
  }
}

void
lod_render_model_t::load_render_model_internal(std::string_view path, lod::detail_level lod_level)
{
  // read file via ASSIMP
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
                                         aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GenBoundingBoxes);
  // check for errors
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
  {
    LOG(INFO) << "[model] : error: '" << importer.GetErrorString() << "'";
    return;
  }
  // retrieve the directory path of the filepath
  directory = path.substr(0, path.find_last_of('/'));

  // process ASSIMP's root node recursively
  process_node(scene->mRootNode, scene, lod_level);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this
// process on its children nodes (if any).
void
lod_render_model_t::process_node(aiNode *node, const aiScene *scene, lod::detail_level lod_level)
{
  // process each mesh located at the current node
  for (usize i = 0; i < node->mNumMeshes; i++)
  {
    // the node object only contains indices to index the actual objects in the scene.
    // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    lod_meshes[(usize)lod_level].meshes.push_back(process_mesh(mesh, scene, lod_level));
  }
  // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for (usize i = 0; i < node->mNumChildren; i++)
  {
    process_node(node->mChildren[i], scene, lod_level);
  }
}

lod_mesh_t
lod_render_model_t::process_mesh(aiMesh *mesh, const aiScene *scene, lod::detail_level lod_level)
{
  // data to fill
  std::vector<lod_mesh_t::vertex_t> vertices;
  std::vector<unsigned int> indices;
  std::vector<renderer::image_tex_lod> textures;

  aiAABB bbox = mesh->mAABB;

  // walk through each of the mesh's vertices
  for (usize i = 0; i < mesh->mNumVertices; i++)
  {
    lod_mesh_t::vertex_t vertex;
    glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly
                      // convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
    // positions
    vector.x        = mesh->mVertices[i].x;
    vector.y        = mesh->mVertices[i].y;
    vector.z        = mesh->mVertices[i].z;
    vertex.position = vector;
    // normals
    if (mesh->HasNormals())
    {
      vector.x      = mesh->mNormals[i].x;
      vector.y      = mesh->mNormals[i].y;
      vector.z      = mesh->mNormals[i].z;
      vertex.normal = vector;
    }
    // texture coordinates
    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
      // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
      vec.x             = mesh->mTextureCoords[0][i].x;
      vec.y             = mesh->mTextureCoords[0][i].y;
      vertex.tex_coords = vec;
      // tangent
      vector.x       = mesh->mTangents[i].x;
      vector.y       = mesh->mTangents[i].y;
      vector.z       = mesh->mTangents[i].z;
      vertex.tangent = vector;
      // bitangent
      vector.x         = mesh->mBitangents[i].x;
      vector.y         = mesh->mBitangents[i].y;
      vector.z         = mesh->mBitangents[i].z;
      vertex.bitangent = vector;
    }
    else
    {
      vertex.tex_coords = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }

  // FIXME: @m256i make cleaner plies

  // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex
  // indices.
  for (usize i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    // retrieve all indices of the face and store them in the indices vector
    for (usize j = 0; j < face.mNumIndices; j++)
    {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (lod_level != lod::detail_level::lod_detail_full)
  {
    std::vector<glm::vec3> temp_verts{};

    for (const auto &vert : vertices)
    {
      temp_verts.push_back(vert.position);
    }

    float threshold           = lod::model_detail_scales[(usize)lod_level];
    size_t target_index_count = size_t(indices.size() * threshold);
    float target_error        = 0.19;
    unsigned int options      = 0; // meshopt_SimplifyX flags, 0 is a safe default

    std::vector<unsigned int> lod(indices.size());
    float lod_error = 0.f;
    lod.resize(meshopt_simplifySloppy(&lod[0], indices.data(), indices.size(), &temp_verts[0].x, temp_verts.size(), sizeof(glm::vec3),
                                      target_index_count, target_error, &lod_error));

    indices = lod;

    LOG(DEBUG) << "simplified mesh with error: " << lod_error;
  }

  // process materials
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
  // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
  // Same applies to other texture as the following list summarizes:
  // diffuse: texture_diffuseN
  // specular: texture_specularN
  // normal: texture_normalN

  // 1. diffuse maps
  std::vector<renderer::image_tex_lod> diffuseMaps = load_mat_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  // 2. specular maps
  std::vector<renderer::image_tex_lod> specularMaps = load_mat_textures(material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  // 3. normal maps
  std::vector<renderer::image_tex_lod> normalMaps = load_mat_textures(material, aiTextureType_NORMALS, "texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
  // 4. height maps
  std::vector<renderer::image_tex_lod> heightMaps = load_mat_textures(material, aiTextureType_HEIGHT, "texture_height");
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  // return a mesh object created from the extracted mesh data
  return lod_mesh_t(vertices, indices, textures, bbox);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<renderer::image_tex_lod>
lod_render_model_t::load_mat_textures(aiMaterial *mat, aiTextureType type, std::string_view typeName)
{
  std::vector<renderer::image_tex_lod> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    bool skip = false;
    for (unsigned int j = 0; j < textures_loaded.size(); j++)
    {
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
      {
        textures.push_back(textures_loaded[j]);
        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
        break;
      }
    }
    if (!skip)
    { // if texture hasn't been loaded already, load it
      renderer::image_tex_lod texture;
      std::string full_name = std::string(this->directory) + "/" + std::string(str.C_Str());

      texture.load(full_name.c_str(), typeName);
      textures.push_back(texture);
      textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't
                                          // unnecessary load duplicate textures.
    }
  }
  return textures;
}
