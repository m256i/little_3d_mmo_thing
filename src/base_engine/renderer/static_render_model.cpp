#include <base_engine/renderer/static_render_model.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <glm/glm.hpp>

#include <string>

#include <base_engine/renderer/core/tex_atlas.h>
#include <stb_image.h>

#include <meshoptimizer.h>
#include <filesystem>

#include <base_engine/renderer/core/math.h>

struct temporary_mesh
{
  struct vert
  {
    glm::vec3 pos, normal;
    glm::vec2 texture_coords;
    glm::vec2 light_map_texture_coords;
  };

  std::string texture_file_path;
  std::vector<vert> vertices;
  std::vector<u32> indices;
  primitives::aabb aabb{};
};

inline temporary_mesh
process_mesh(aiMesh *mesh, const aiScene *scene)
{
  temporary_mesh out;

  out.aabb = {{mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z}, {mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z}};

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

std::pair<glm::vec3, glm::vec3>
compute_obb_bounds(const std::vector<glm::vec3> &vertices, const glm::vec3 &centroid, const glm::vec3 &axisU, const glm::vec3 &axisV,
                   const glm::vec3 &axisW)
{
  // Initialize min and max projection values
  float minU = std::numeric_limits<float>::max();
  float maxU = std::numeric_limits<float>::lowest();

  float minV = std::numeric_limits<float>::max();
  float maxV = std::numeric_limits<float>::lowest();

  float minW = std::numeric_limits<float>::max();
  float maxW = std::numeric_limits<float>::lowest();

  // Iterate over all vertices and compute projections on each axis
  for (const auto &vertex : vertices)
  {
    glm::vec3 deviation = vertex - centroid;

    // Project onto each axis (dot product)
    float projectionU = glm::dot(deviation, axisU);
    float projectionV = glm::dot(deviation, axisV);
    float projectionW = glm::dot(deviation, axisW);

    // Update min and max for each axis
    if (projectionU < minU) minU = projectionU;
    if (projectionU > maxU) maxU = projectionU;

    if (projectionV < minV) minV = projectionV;
    if (projectionV > maxV) maxV = projectionV;

    if (projectionW < minW) minW = projectionW;
    if (projectionW > maxW) maxW = projectionW;
  }

  // Compute half-sizes along each axis
  return {glm::vec3{minU, minV, minW}, glm::vec3{maxU, maxV, maxW}};
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

  std::vector<u8 *> pointers;
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

    pointers.push_back(texture_buf);

    atlas.add_texture(fnv1a::hash(tex.c_str()), texture_buf, tex_sx, tex_sy, num_channels);

    LOG(INFO) << "num channels: " << num_channels;
  }

  /*
  TODO: if we only have once texture we dont even want an atlas
  */

  pipeline.add_uniform_callack("model", [&]() { return model; });
  pipeline.add_uniform_callack("projection", [&]() { return projection; });
  pipeline.add_uniform_callack("view", [&]() { return view; });

  auto atlas_pointer = atlas.generate();

  pipeline.load_texture_from_memory("texture_diffuse1", atlas_pointer, atlas.atlas_width, atlas.atlas_height, atlas.num_channels);
  pipeline.initialize();

  /*
  this is a memory leak free work environment!
  */
  for (const auto p : pointers)
  {
    stbi_image_free(p);
  }

  u32 current_index_offset{}, total_vertex_count{};

  glm::vec3 centroid{};
  glm::mat3 covariance_matrix{};

  std::vector<glm::vec3> temp_verts;

  for (auto &mesh : meshes)
  {
    for (auto &vert : mesh.vertices)
    {

      auto uv_map = atlas.get_uv_map(fnv1a::hash(mesh.texture_file_path.c_str()), vert.texture_coords.x, vert.texture_coords.y);
      /* add the atlas index to the UV here */
      vert.texture_coords.x = uv_map.first;
      vert.texture_coords.y = uv_map.second;

      temp_verts.push_back(vert.pos);
      pipeline.push_back_vertex<temporary_mesh::vert>(vert);
      total_vertex_count++;

      centroid += vert.pos;
    }
    for (const u32 index : mesh.indices)
    {
      pipeline.push_back_index(index + current_index_offset);
    }
    current_index_offset += mesh.indices.size();
  }

  centroid /= total_vertex_count;

  for (auto &mesh : meshes)
  {
    for (auto &vert : mesh.vertices)
    {
      glm::vec3 deviation = vert.pos - centroid;
      covariance_matrix += glm::outerProduct(deviation, deviation); // outerProduct is glm::mat3
    }
  }

  covariance_matrix /= total_vertex_count;

  this->bounding_box.center = centroid;

  auto eigen_vectors = core::math::eigen(covariance_matrix);
  if (eigen_vectors.has_value())
  {
    eigen_vectors.value()[0] = glm::normalize(eigen_vectors.value()[0]);
    eigen_vectors.value()[1] = glm::normalize(eigen_vectors.value()[1]);
    eigen_vectors.value()[2] = glm::normalize(eigen_vectors.value()[2]);

    bounding_box.axes = eigen_vectors.value();

    glm::vec3 U, V, W;
    U = eigen_vectors.value()[0];
    V = eigen_vectors.value()[1];
    W = eigen_vectors.value()[2];

    auto dimensions = compute_obb_bounds(temp_verts, centroid, U, V, W);

    bounding_box.mins = dimensions.first;
    bounding_box.maxs = dimensions.second;
  }

  translated_bbox = bounding_box;

  std::vector<u32> optimized_indices(pipeline.vbuf.indices.size());
  std::vector<u8> optimized_vertices(pipeline.vbuf.raw_buffer.size());

  /* optimize the mesh */
  meshopt_optimizeVertexCache(optimized_indices.data(), pipeline.vbuf.indices.data(), pipeline.vbuf.indices.size(), total_vertex_count);

  std::vector<u32> optimized_indices2(optimized_indices.size());

  meshopt_optimizeOverdraw(optimized_indices2.data(), optimized_indices.data(), optimized_indices.size(),
                           (f32 *)std::launder(pipeline.vbuf.raw_buffer.data()), total_vertex_count,
                           pipeline.vbuf.get_total_attribute_stride(), 1.15f);

  meshopt_optimizeVertexFetch(optimized_vertices.data(), optimized_indices.data(), optimized_indices.size(),
                              pipeline.vbuf.raw_buffer.data(), total_vertex_count, pipeline.vbuf.get_total_attribute_stride());

  pipeline.vbuf.indices    = optimized_indices;
  pipeline.vbuf.raw_buffer = optimized_vertices;

  /*
  TODO: overdraw optimization
  */

  atlas.destroy();
  pipeline.setup_drawbuffer();
}

u0
static_render_model_lod::load_from_file(std::string_view _path, lod::detail_level _detail_level)
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

  std::string directory = std::string{_path.substr(0, _path.find_last_of('/'))};

  std::vector<temporary_mesh> meshes;
  process_node(scene->mRootNode, scene, meshes);

  // assert(false);

  std::vector<u8 *> pointers;
  core::textures::texture_atlas atlas;

  /* load all textures and pack 'em */
  for (const auto &mesh : meshes)
  {
    auto &tex = mesh.texture_file_path;

    std::string full_path = directory + "/" + tex;

    usize lc1 = full_path.find("\\");
    while (lc1 != std::string::npos)
    {
      full_path.replace(lc1, 1, "/");
      lc1 = full_path.find("\\", lc1);
    }

    i32 tex_sx, tex_sy, num_channels;
    u8 *texture_buf = stbi_load(full_path.c_str(), &tex_sx, &tex_sy, &num_channels, STBI_default);

    if (!texture_buf)
    {
      LOG(INFO) << "failed to load texture: " << (directory + tex);
      LOG(INFO) << "working directory of executable: " << std::filesystem::current_path();
      assert(false);
    }

    pointers.push_back(texture_buf);

    glm::vec2 min_uv = glm::vec2{std::numeric_limits<f32>::max()};
    glm::vec2 max_uv = glm::vec2{std::numeric_limits<f32>::min()};

    /*
    calculate the minimum and maxium UV values to add repeats to the texture atlas
    */
    for (const auto &vert : mesh.vertices)
    {
      min_uv = glm::vec2{std::min(min_uv.x, vert.texture_coords.x), std::min(min_uv.y, vert.texture_coords.y)};
      max_uv = glm::vec2{std::max(max_uv.x, vert.texture_coords.x), std::max(max_uv.y, vert.texture_coords.y)};
    }

    LOG(INFO) << "adding tex to atlas!";

    atlas.add_texture(fnv1a::hash(tex.c_str()), texture_buf, tex_sx, tex_sy, num_channels, min_uv, max_uv);
    LOG(INFO) << "addeded tex to atlas!";
    LOG(INFO) << "num channels: " << num_channels;
  }

  /*
  TODO: if we only have once texture we dont even want an atlas
  */

  pipeline.add_uniform_callack("model", [&]() { return model; });
  pipeline.add_uniform_callack("projection", [&]() { return projection; });
  pipeline.add_uniform_callack("view", [&]() { return view; });

  auto atlas_pointer = atlas.generate();

  core::textures::texture_atlas lighting_atlas{};

  using namespace lighting::static_lighting;
  std::vector<static_lighting_t::tri_shade_t> lightmap_textures{};

  struct index_uv
  {
    usize tri_index;
    glm::vec2 uv;
  };

  std::unordered_map<usize, index_uv> lightUV_vertex_map;

  usize tri_index = 1;
  for (auto &mesh : meshes)
  {
    for (usize index = 0; index <= mesh.indices.size() - 3; index += 3)
    {
      usize idx  = mesh.indices[index];
      usize idx2 = mesh.indices[index + 1];
      usize idx3 = mesh.indices[index + 2];

      glm::vec3 triA = mesh.vertices[idx].pos;
      glm::vec3 triB = mesh.vertices[idx2].pos;
      glm::vec3 triC = mesh.vertices[idx3].pos;

      glm::vec3 norm = mesh.vertices[idx].normal + mesh.vertices[idx2].normal + mesh.vertices[idx3].normal;

      norm /= 3.f;
      norm = glm::normalize(norm);

      static_lighting_t::triangle tri{triA, triB, triC, norm};

      auto shade = lighting_thing.shade_triangle(tri);

      lightUV_vertex_map[idx].uv        = shade.uvA;
      lightUV_vertex_map[idx].tri_index = tri_index;

      lightUV_vertex_map[idx2].uv        = shade.uvB;
      lightUV_vertex_map[idx2].tri_index = tri_index;

      lightUV_vertex_map[idx3].uv        = shade.uvC;
      lightUV_vertex_map[idx3].tri_index = tri_index;

      lightmap_textures.push_back(shade);
      lighting_atlas.add_texture(tri_index, (u8 *)lightmap_textures.back().pixel_data.data(), lightmap_textures.back().tex_w,
                                 lightmap_textures.back().tex_h, 4);

      tri_index++;
    }
  }

  auto lighting_atlas_pointer = lighting_atlas.generate();

  pipeline.load_texture_from_memory("texture_baked_light1", lighting_atlas_pointer, lighting_atlas.atlas_width, lighting_atlas.atlas_height,
                                    lighting_atlas.num_channels);

  pipeline.load_texture_from_memory("texture_diffuse1", atlas_pointer, atlas.atlas_width, atlas.atlas_height, atlas.num_channels);

  pipeline.initialize();

  puts("pipeline initialized");

  /*
  this is a memory leak free work environment!
  */
  for (const auto p : pointers)
  {
    stbi_image_free(p);
  }

  u32 current_index_offset{}, total_vertex_count{};

  glm::vec3 centroid{};
  glm::mat3 covariance_matrix{};

  std::vector<glm::vec3> temp_verts;

  for (auto &mesh : meshes)
  {
    for (usize i = 0; i < mesh.indices.size() - 3; i += 3)
    {
      const auto idx1 = mesh.indices[i];
      const auto idx2 = mesh.indices[i + 1];
      const auto idx3 = mesh.indices[i + 2];

      const auto vert1 = mesh.vertices[idx1];
      const auto vert2 = mesh.vertices[idx2];
      const auto vert3 = mesh.vertices[idx3];

      auto UV1 = atlas.get_uv_map(fnv1a::hash(mesh.texture_file_path.c_str()), vert1.texture_coords.x, vert1.texture_coords.y);
      auto UV2 = atlas.get_uv_map(fnv1a::hash(mesh.texture_file_path.c_str()), vert2.texture_coords.x, vert2.texture_coords.y);
      auto UV3 = atlas.get_uv_map(fnv1a::hash(mesh.texture_file_path.c_str()), vert3.texture_coords.x, vert3.texture_coords.y);

      atlas.add_debug_triangle(glm::vec2{UV1.first, UV1.second}, glm::vec2{UV2.first, UV2.second}, glm::vec2{UV3.first, UV3.second});
    }

    for (const u32 index : mesh.indices)
    {
      assert(lightUV_vertex_map.contains(index));

      auto new_uvs =
          lighting_atlas.get_uv_map(lightUV_vertex_map.at(index).tri_index, lightUV_vertex_map[index].uv.x, lightUV_vertex_map[index].uv.y);

      mesh.vertices[index].light_map_texture_coords = glm::vec2{new_uvs.first, new_uvs.second};

      pipeline.push_back_index(index + current_index_offset);
    }
    for (auto &vert : mesh.vertices)
    {
      auto uv_map = atlas.get_uv_map(fnv1a::hash(mesh.texture_file_path.c_str()), vert.texture_coords.x, vert.texture_coords.y);
      /* add the atlas index to the UV here */

      vert.texture_coords.x = uv_map.first;
      vert.texture_coords.y = uv_map.second;

      temp_verts.push_back(vert.pos);

      pipeline.push_back_vertex<temporary_mesh::vert>(vert);

      total_vertex_count++;

      centroid += vert.pos;
    }
    // assert(false);
    current_index_offset += mesh.indices.size();
  }
  LOG(INFO) << "done with lioghtmap shit";

  pipeline.load_texture_from_memory("texture_diffuse1", atlas.debug_texture_pointer, atlas.atlas_width, atlas.atlas_height,
                                    atlas.num_channels);

  atlas_debug_tex.load_from_mem(atlas.debug_texture_pointer, atlas.atlas_width, atlas.atlas_height, 4);

  // assert(false);

  LOG(INFO) << "created debug atlas texture";

  centroid /= total_vertex_count;

  for (auto &mesh : meshes)
  {
    for (auto &vert : mesh.vertices)
    {
      glm::vec3 deviation = vert.pos - centroid;
      covariance_matrix += glm::outerProduct(deviation, deviation); // outerProduct is glm::mat3
    }
  }

  covariance_matrix /= total_vertex_count;

  this->bounding_box.center = centroid;

  auto eigen_vectors = core::math::eigen(covariance_matrix);
  if (eigen_vectors.has_value())
  {
    eigen_vectors.value()[0] = glm::normalize(eigen_vectors.value()[0]);
    eigen_vectors.value()[1] = glm::normalize(eigen_vectors.value()[1]);
    eigen_vectors.value()[2] = glm::normalize(eigen_vectors.value()[2]);

    bounding_box.axes = eigen_vectors.value();

    glm::vec3 U, V, W;
    U = eigen_vectors.value()[0];
    V = eigen_vectors.value()[1];
    W = eigen_vectors.value()[2];

    auto dimensions = compute_obb_bounds(temp_verts, centroid, U, V, W);

    bounding_box.mins = dimensions.first;
    bounding_box.maxs = dimensions.second;
  }

  LOG(INFO) << "calculated OBB";

  // f32 threshold            = lod::model_detail_scales[(usize)_detail_level];
  // usize target_index_count = usize(pipeline.vbuf.indices.size() * threshold);
  // f32 target_error         = 0.15;
  // std::vector<u32> lod(pipeline.vbuf.indices.size());
  // f32 lod_error = 0.f;
  // lod.resize(meshopt_simplifySloppy(&lod[0], pipeline.vbuf.indices.data(), pipeline.vbuf.indices.size(),
  //                                   (f32 *)std::launder(pipeline.vbuf.raw_buffer.data()), total_vertex_count,
  //                                   pipeline.vbuf.get_total_attribute_stride(), target_index_count, target_error, &lod_error));
  // pipeline.vbuf.indices = lod;
  // std::vector<u32> optimized_indices(pipeline.vbuf.indices.size());
  // std::vector<u8> optimized_vertices(pipeline.vbuf.raw_buffer.size());
  // /* optimize the mesh */
  // meshopt_optimizeVertexCache(optimized_indices.data(), pipeline.vbuf.indices.data(), pipeline.vbuf.indices.size(), total_vertex_count);
  // std::vector<u32> optimized_indices2(optimized_indices.size());
  // meshopt_optimizeOverdraw(optimized_indices2.data(), optimized_indices.data(), optimized_indices.size(),
  //                          (f32 *)std::launder(pipeline.vbuf.raw_buffer.data()), total_vertex_count,
  //                          pipeline.vbuf.get_total_attribute_stride(), 1.15f);
  // meshopt_optimizeVertexFetch(optimized_vertices.data(), optimized_indices.data(), optimized_indices.size(),
  //                             pipeline.vbuf.raw_buffer.data(), total_vertex_count, pipeline.vbuf.get_total_attribute_stride());
  // pipeline.vbuf.indices    = optimized_indices;
  // pipeline.vbuf.raw_buffer = optimized_vertices;

  /*
  TODO: overdraw optimization
  */

  atlas.destroy();
  pipeline.setup_drawbuffer();

  /*

  */
  LOG(INFO) << "loaded model succesfully!";
}

u0
instanced_static_render_model::load_from_file(std::string_view _path)
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

  std::vector<u8 *> pointers;
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

    pointers.push_back(texture_buf);

    atlas.add_texture(fnv1a::hash(tex.c_str()), texture_buf, tex_sx, tex_sy, num_channels);

    LOG(INFO) << "num channels: " << num_channels;
  }

  /*
  TODO: if we only have once texture we dont even want an atlas
  */

  pipeline.add_uniform_callack("projection", [&]() { return projection; });
  pipeline.add_uniform_callack("view", [&]() { return view; });

  auto atlas_pointer = atlas.generate();

  pipeline.load_texture_from_memory("texture_diffuse1", atlas_pointer, atlas.atlas_width, atlas.atlas_height, atlas.num_channels);

  LOG(INFO) << "Lolaz0";

  pipeline.initialize();

  LOG(INFO) << "Lolaz1";
  /*
  this is a memory leak free work environment!
  */
  for (const auto p : pointers)
  {
    if (p) stbi_image_free(p);
  }

  u32 current_index_offset{}, total_vertex_count{};
  for (auto &mesh : meshes)
  {
    for (auto &vert : mesh.vertices)
    {
      auto uv_map = atlas.get_uv_map(fnv1a::hash(mesh.texture_file_path.c_str()), vert.texture_coords.x, vert.texture_coords.y);
      /* add the atlas index to the UV here */
      vert.texture_coords.x = uv_map.first;
      vert.texture_coords.y = uv_map.second;

      pipeline.push_back_vertex<temporary_mesh::vert>(vert);
      total_vertex_count++;
    }
    for (const u32 index : mesh.indices)
    {
      pipeline.push_back_index(index + current_index_offset);
    }
    current_index_offset += mesh.indices.size();
  }

  std::vector<u32> optimized_indices(pipeline.vbuf.indices.size());
  std::vector<u8> optimized_vertices(pipeline.vbuf.raw_buffer.size());

  /* optimize the mesh */
  meshopt_optimizeVertexCache(optimized_indices.data(), pipeline.vbuf.indices.data(), pipeline.vbuf.indices.size(), total_vertex_count);

  std::vector<u32> optimized_indices2(optimized_indices.size());

  meshopt_optimizeOverdraw(optimized_indices2.data(), optimized_indices.data(), optimized_indices.size(),
                           (f32 *)std::launder(pipeline.vbuf.raw_buffer.data()), total_vertex_count,
                           pipeline.vbuf.get_total_attribute_stride(), 1.15f);

  meshopt_optimizeVertexFetch(optimized_vertices.data(), optimized_indices.data(), optimized_indices.size(),
                              pipeline.vbuf.raw_buffer.data(), total_vertex_count, pipeline.vbuf.get_total_attribute_stride());

  pipeline.vbuf.indices    = optimized_indices;
  pipeline.vbuf.raw_buffer = optimized_vertices;

  /*
  TODO: overdraw optimization
  */

  atlas.destroy();
  pipeline.setup_drawbuffer();
}

u0
instanced_static_render_model_lod::load_from_file(std::string_view _path, lod::detail_level _detail_level)
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

  std::vector<u8 *> pointers;
  core::textures::texture_atlas atlas;

  /* load all textures and pack 'em */
  for (const auto &mesh : meshes)
  {
    this->aabb.expand_to_bbox(mesh.aabb.min, mesh.aabb.max);
    auto &tex = mesh.texture_file_path;

    i32 tex_sx, tex_sy, num_channels;
    u8 *texture_buf = stbi_load((directory + "/" + tex).c_str(), &tex_sx, &tex_sy, &num_channels, STBI_default);

    if (!texture_buf)
    {
      LOG(INFO) << "failed to load texture: " << (directory + "/" + tex);
      assert(false);
    }

    pointers.push_back(texture_buf);

    atlas.add_texture(fnv1a::hash(tex.c_str()), texture_buf, tex_sx, tex_sy, num_channels);

    LOG(INFO) << "num channels: " << num_channels;
  }

  /*
  TODO: if we only have once texture we dont even want an atlas
  */

  pipeline.add_uniform_callack("projection", [&]() { return projection; });
  pipeline.add_uniform_callack("view", [&]() { return view; });

  auto atlas_pointer = atlas.generate();

  pipeline.load_texture_from_memory("texture_diffuse1", atlas_pointer, atlas.atlas_width, atlas.atlas_height, atlas.num_channels);

  LOG(INFO) << "Lolaz0";

  pipeline.initialize();

  LOG(INFO) << "Lolaz1";
  /*
  this is a memory leak free work environment!
  */
  for (const auto p : pointers)
  {
    if (p) stbi_image_free(p);
  }

  u32 current_index_offset{}, total_vertex_count{};
  glm::vec3 centroid{};
  glm::mat3 covariance_matrix{};

  std::vector<glm::vec3> temp_verts;

  for (auto &mesh : meshes)
  {
    for (auto &vert : mesh.vertices)
    {

      auto uv_map = atlas.get_uv_map(fnv1a::hash(mesh.texture_file_path.c_str()), vert.texture_coords.x, vert.texture_coords.y);
      /* add the atlas index to the UV here */
      vert.texture_coords.x = uv_map.first;
      vert.texture_coords.y = uv_map.second;

      temp_verts.push_back(vert.pos);
      pipeline.push_back_vertex<temporary_mesh::vert>(vert);
      total_vertex_count++;

      centroid += vert.pos;
    }
    for (const u32 index : mesh.indices)
    {
      pipeline.push_back_index(index + current_index_offset);
    }
    current_index_offset += mesh.indices.size();
  }

  centroid /= total_vertex_count;

  for (auto &mesh : meshes)
  {
    for (auto &vert : mesh.vertices)
    {
      glm::vec3 deviation = vert.pos - centroid;
      covariance_matrix += glm::outerProduct(deviation, deviation); // outerProduct is glm::mat3
    }
  }

  covariance_matrix /= total_vertex_count;

  this->bounding_box.center = centroid;

  auto eigen_vectors = core::math::eigen(covariance_matrix);
  if (eigen_vectors.has_value())
  {
    eigen_vectors.value()[0] = glm::normalize(eigen_vectors.value()[0]);
    eigen_vectors.value()[1] = glm::normalize(eigen_vectors.value()[1]);
    eigen_vectors.value()[2] = glm::normalize(eigen_vectors.value()[2]);

    bounding_box.axes = eigen_vectors.value();

    glm::vec3 U, V, W;
    U = eigen_vectors.value()[0];
    V = eigen_vectors.value()[1];
    W = eigen_vectors.value()[2];

    auto dimensions = compute_obb_bounds(temp_verts, centroid, U, V, W);

    bounding_box.mins = dimensions.first;
    bounding_box.maxs = dimensions.second;
  }

  f32 threshold            = lod::model_detail_scales[(usize)_detail_level];
  usize target_index_count = usize(pipeline.vbuf.indices.size() * threshold);
  f32 target_error         = 0.2;

  std::vector<u32> lod(pipeline.vbuf.indices.size());
  f32 lod_error = 0.f;
  lod.resize(meshopt_simplifySloppy(&lod[0], pipeline.vbuf.indices.data(), pipeline.vbuf.indices.size(),
                                    (f32 *)std::launder(pipeline.vbuf.raw_buffer.data()), total_vertex_count,
                                    pipeline.vbuf.get_total_attribute_stride(), target_index_count, target_error, &lod_error));

  pipeline.vbuf.indices = lod;

  std::vector<u32> optimized_indices(pipeline.vbuf.indices.size());
  std::vector<u8> optimized_vertices(pipeline.vbuf.raw_buffer.size());

  /* optimize the mesh */
  meshopt_optimizeVertexCache(optimized_indices.data(), pipeline.vbuf.indices.data(), pipeline.vbuf.indices.size(), total_vertex_count);

  std::vector<u32> optimized_indices2(optimized_indices.size());

  meshopt_optimizeOverdraw(optimized_indices2.data(), optimized_indices.data(), optimized_indices.size(),
                           (f32 *)std::launder(pipeline.vbuf.raw_buffer.data()), total_vertex_count,
                           pipeline.vbuf.get_total_attribute_stride(), 1.15f);

  meshopt_optimizeVertexFetch(optimized_vertices.data(), optimized_indices.data(), optimized_indices.size(),
                              pipeline.vbuf.raw_buffer.data(), total_vertex_count, pipeline.vbuf.get_total_attribute_stride());

  pipeline.vbuf.indices    = optimized_indices;
  pipeline.vbuf.raw_buffer = optimized_vertices;

  /*
  TODO: overdraw optimization
  */

  atlas.destroy();
  pipeline.setup_drawbuffer();
}

} // namespace renderer