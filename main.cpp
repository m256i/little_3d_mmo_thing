#include <cmath>
#include <deque>
#include <float.h>
#include <limits>
#include <numeric>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include <stb_image.h>
#include <unordered_map>
#include <utility>
#include "base_engine/physics_system/chull.h"
#include "base_engine/renderer/model_renderer.h"
#include "common.h"

#include "assimp/aabb.h"
#include "assimp/vector3.h"
#include "common.h"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "headers/base_engine/renderer/model_renderer.h"
#include "headers/window.h"

#include "headers/base_engine/physics_system/bbox.h"
#include "headers/base_engine/renderer/shader.h"

#include "headers/base_engine/renderer/game_renderer.h"
#include "headers/base_engine/physics_system/mesh_collider.h"
#include "headers/base_engine/physics_system/chull.h"
#include "headers/base_engine/physics_system/mesh_chunking.h"

#include "headers/base_engine/debug/debug_menu.h"
#include "headers/base_engine/debug/debug_overlay.h"

#include "headers/logging/easylogging++.h"
#include <include/vhacd/wavefront.h>

#include <base_engine/world_generation/ground_plane.h>
#include <base_engine/core/draw_data_buffer.h>

constinit f32 lastX = 1920.f / 2.0f;
constinit f32 lastY = 1080.f / 2.0f;

static f32 movement_speed = 10;

static bool in_menu           = false;
constinit bool adjusted_mouse = false;
double last_change_time       = 0.0f;

bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

game_renderer_t game_renderer{};

auto
mouse_callback([[maybe_unused]] GLFWwindow* window, double xpos_, double ypos_) -> u0
{
  if (!in_menu)
  {
    game_renderer.game_camera.process_mouse_movement(-(lastX - xpos_), lastY - ypos_);
    lastX = (f32)xpos_;
    lastY = (f32)ypos_;
  }
}

void
process_input(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    if (glfwGetTime() - last_change_time > 0.2)
    {
      in_menu = !in_menu;
      puts("changed menu state");
      last_change_time = glfwGetTime();
    }
  };

  if (!in_menu)
  {
    using mt = render_camera_t::e_movement_types;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      game_renderer.game_camera.process_keyboard_input(mt::forward, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      game_renderer.game_camera.process_keyboard_input(mt::backward, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      game_renderer.game_camera.process_keyboard_input(mt::left, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      game_renderer.game_camera.process_keyboard_input(mt::right, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      game_renderer.game_camera.process_keyboard_input(mt::up, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      game_renderer.game_camera.process_keyboard_input(mt::down, movement_speed * deltaTime);
  }
}

/* TODO:
remove stupi monad thing
input system
*/

inline void
draw_tree_recursive(const tree_node_t& branch)
{
  debug_overlay_t::draw_AABB(branch.bbox.min, branch.bbox.max, 0xff04f0ff, true);
  for (const auto& ite : branch.children)
  {
    if (ite != nullptr)
    {
      draw_tree_recursive(*ite);
    }
  }
}

INITIALIZE_EASYLOGGINGPP

auto
main(i32 argc, char** argv) -> i32
{
  START_EASYLOGGINGPP(argc, argv);

  std::unordered_map<u32, debug_bbox_t> debug_boxes{};
  debug_menu_t debug_menu{};
  partial_spacial_tree_t tree{};

  std::unordered_map<u32, voxel_grid_t> vgrids;

  ground_plane_t plane{};

  // clang-format off
  
  // usage: 
  static draw_buf_t
  <
    buf_elem<"stupi idior", float>,
    buf_elem<"maow!", std::tuple<int, int, int>>
  > 
  impl_test_buffer;
  
  static_assert(impl_test_buffer.index_of_str<"maow!">() == 1); // OK!
  //static_assert(impl_test_buffer.index_of_str<"maaow!">() == 1); // COMPILE ERROR!
  
  impl_test_buffer.load<"maow!">(std::tuple{1,2,3});
  impl_test_buffer.load<"maodw!">(std::tuple{1,2,3}); // COMPILE ERROR!

  // clang-format on

  return create_window("WoW Clone :D", false)
      .register_callback(glfwSetCursorPosCallback, mouse_callback)
      .init(
          [&](GLFWwindow* _window)
          {
            debug_menu.init_menu(_window);
            debug_overlay_t::init(_window, game_renderer.game_camera);

            game_renderer.update_frame_buffer(_window);
            // game_renderer.model_renderer.add_model("dungeon", "../data/blackrock_lower_instance.obj");

            // for (const auto& mesh : game_renderer.model_renderer.static_world_models.at("dungeon").draw_model.meshes)
            //{
            //   std::vector<triangle_t> tris;
            //
            //   for (usize i = 0; i < mesh.indices.size() - 4; i += 3)
            //   {
            //     tris.push_back({glm::vec3{mesh.vertices[mesh.indices[i]].position}, glm::vec3{mesh.vertices[mesh.indices[i +
            //     1]].position},
            //                     glm::vec3{mesh.vertices[mesh.indices[i + 2]].position}});
            //   }
            //   LOG(INFO) << "generating done mesh!";
            //
            //   vgrids[mesh.VAO].setup(20, 20, 20);
            //   vgrids[mesh.VAO].generate(mesh.bbox, tris, tris.size());
            // }

            // TODO: system to make here! should be set once when all of the tesselation shit gets drawn
            glPatchParameteri(GL_PATCH_VERTICES, 4);

            plane.load_shader();
            plane.initialize(8);
          })
      .loop(
          [&](GLFWwindow* _window)
          {
            glClearColor(55.f / 255.f, 55.f / 255.f, 182.f / 255.f, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            process_input(_window);

            f32 currentFrame = (f32)glfwGetTime();
            deltaTime        = currentFrame - lastFrame;
            lastFrame        = currentFrame;

            f64 cX, cY;
            glfwGetCursorPos(_window, &cX, &cY);

            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            // game_renderer.render();

            // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // auto collision_meshes = tree.find(game_renderer.game_camera.vec_position);

            auto& cam = game_renderer.game_camera;
            plane.draw(game_renderer.display_w, game_renderer.display_h, &game_renderer.game_camera, 0xffffffff);

            glPointSize(5);

            for (auto& vgrid : vgrids)
            {
              // vgrid.second.draw();
              //  if (auto voxel = vgrid.second.find(cam.vec_position); voxel != nullptr)
              //{
              //    debug_overlay_t::draw_AABB(voxel->bbox.min, voxel->bbox.max, 0x0302ffff, true);
              //    for (const auto& tri : voxel->triangles_inside)
              //    {
              //      debug_overlay_t::draw_point(tri.a, 0xff0000ff, true);
              //      debug_overlay_t::draw_point(tri.b, 0xff0000ff, true);
              //      debug_overlay_t::draw_point(tri.c, 0xff0000ff, true);
              //    }
              //  }

              // auto largest_cube = vgrid.second.fint_largest_sub_cuboid(vgrid.second.grid);
              // debug_overlay_t::draw_AABB(largest_cube.first.min, largest_cube.first.max, 0x0202ffff, true);
            }

            glPointSize(1);

            debug_menu.print_stdcout();
            debug_menu.draw(_window, in_menu, deltaTime);
          })
      .stdexit();
}