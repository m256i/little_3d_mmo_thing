#pragma once

#include <common.h>

#include "ground_mesh.h"
#include "../renderer/instanced_model.h"
#include "../debug/debug_overlay.h"

namespace world_gen
{
struct ground_chunk
{
  f32 scale_xyz = 100.f;
  glm::vec3 world_position{0, 0, 0};
  instanced_static_world_model test_foliage_model{1};

  u0
  init(i32 chunk_scale, debug_menu_t& debug_menu, glm::vec3 _coords = glm::vec3{0, 0, 0})
  {

    debug_menu.add_debug_widget("terrain", debug_menu_t::debug_widget_type::button, "regenerate",
                                [&]([[maybe_unused]] f64) { this->regenerate(debug_menu); });

    scale_xyz      = chunk_scale;
    world_position = _coords;

    /*
    set up which foliage models we even want in this chunk here later just testin for now
    */

    /*
    do this for all instanced models the scrip asks for
    */
    test_foliage_model.load_model("../data/foliage/vineyard/gilneas_lumpygrass01.obj");
    test_foliage_model.init_shader("../basic_model_instanced.vs", "../basic_model_instanced.fs");

    mesh.initialize(0, chunk_scale, debug_menu, _coords);
    mesh.load_shader();

    /* loop over triangles in mesh */

    for (usize i = 0; i < mesh.verts.size() / 3; i = i + 3)
    {
      auto& v0 = mesh.verts[i].vert;
      auto& v1 = mesh.verts[i + 1].vert;
      auto& v2 = mesh.verts[i + 2].vert;

      auto& n0 = mesh.verts[i].normal;
      auto& n1 = mesh.verts[i + 1].normal;
      auto& n2 = mesh.verts[i + 2].normal;

      // normal of the triangle
      auto trinorm = (n0 + n1 + n2) / 3.f;

      debug_triangles.push_back({.v0 = v0, .v1 = v1, .v2 = v2, .normal = trinorm});
    }
  }

  struct dbg_tri
  {
    glm::vec3 v0, v1, v2;
    glm::vec3 normal;
  };

  struct test_foliage_point
  {
    glm::vec3 location, normal;
  };

  std::vector<test_foliage_point> foliage_points;

  u0
  regenerate(debug_menu_t& dmenu)
  {
    puts("regenerate called");

    test_foliage_model.set_instance_count(0);

    foliage_points.clear();
    debug_triangles.clear();
    mesh.regenerate(scale_xyz, dmenu);

    for (usize i = 0; i < mesh.indices.size(); i = i + 3)
    {
      mesh.world_position  = world_position;
      const auto world_pos = world_position;

      auto v0 = mesh.verts[mesh.indices[i]].vert * scale_xyz + world_pos;
      auto v1 = mesh.verts[mesh.indices[i + 1]].vert * scale_xyz + world_pos;
      auto v2 = mesh.verts[mesh.indices[i + 2]].vert * scale_xyz + world_pos;

      auto& n0 = mesh.verts[mesh.indices[i]].normal;
      auto& n1 = mesh.verts[mesh.indices[i + 1]].normal;
      auto& n2 = mesh.verts[mesh.indices[i + 2]].normal;

      // normal of the triangle
      auto trinorm = (n0 + n1 + n2) / 3.f;

      debug_triangles.push_back({.v0 = v0, .v1 = v1, .v2 = v2, .normal = trinorm});

      std::cout << "trinorm: " << trinorm.x << " " << trinorm.y << " " << trinorm.z << "\n";

      if (trinorm.y > 0.8)
      {
        foliage_points.push_back({.location = (v0 + v1 + v2) / 3.f, .normal = trinorm});
      }
    }

    test_foliage_model.set_instance_count(foliage_points.size());

    usize i = 0;
    for (auto& point : foliage_points)
    {
      std::cout << "generating grass on point: " << point.location.x << " " << point.location.y << "" << point.location.z << "\n";
      test_foliage_model.get_instance_data()[i].world_position = point.location;
      test_foliage_model.get_instance_data()[i].world_rotation = point.normal;
      test_foliage_model.get_instance_data()[i].world_scale    = glm::vec3{10};
      ++i;
    }

    test_foliage_model.reapply_all_translations();
    test_foliage_model.buffer();
  }

  u0
  draw(auto& proj, auto& view, auto display_w, auto display_h, auto& camera)
  {
    // i32 I = 0;
    // for (const auto& tri : debug_triangles)
    // {
    //   // debug_overlay_t::draw_triangle({tri.v0, tri.v1, tri.v2}, 0xff00ffff, true);
    //   // debug_overlay_t::draw_point(tri.v0, 0xffffffff, true);
    //   // debug_overlay_t::draw_point(tri.v1, 0xffffffff, true);
    //   // debug_overlay_t::draw_point(tri.v2, 0xffffffff, true);
    //   if (I % 7 == 0)
    //   {
    //     auto tri_center = (tri.v0 + tri.v1 + tri.v2) / 3.f;
    //     glPointSize(5.f);
    //     glLineWidth(3.f);
    //     debug_overlay_t::draw_line({tri_center, tri_center + (tri.normal * 45.f)}, 0xffffffff, true);
    //     glPointSize(1.f);
    //     glLineWidth(1.f);
    //   }
    //   ++I;
    // }

    mesh.draw(display_w, display_h, camera, lod::detail_level::lod_detail_full, 0xffffffff);
    test_foliage_model.draw(proj, view);
  }

  std::vector<dbg_tri> debug_triangles;

  std::vector<instanced_static_world_model> foliage_models;
  ground_mesh_chunk_t mesh;
};

} // namespace world_gen