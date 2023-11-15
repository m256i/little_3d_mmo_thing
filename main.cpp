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

#include "headers/base_engine/debug/debug_menu.h"
#include "headers/base_engine/debug/debug_overlay.h"

#include "headers/logging/easylogging++.h"

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

  std::unordered_map<u32, convex_hull_t> sub_hulls;

  return create_window("WoW Clone :D", false)
      .register_callback(glfwSetCursorPosCallback, mouse_callback)
      .init(
          [&](GLFWwindow* _window)
          {
            debug_menu.init_menu(_window);
            debug_overlay_t::init(_window, game_renderer.game_camera);

            game_renderer.update_frame_buffer(_window);
            game_renderer.model_renderer.add_model("dungeon", "../data/duskwoodchapel.obj");
            tree.generate(game_renderer.model_renderer.static_world_models.at("dungeon").draw_model.meshes);
          })
      .loop(
          [&](GLFWwindow* _window)
          {
            glClearColor(0.1f, 0.1f, 0.1f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            process_input(_window);

            f32 currentFrame = (f32)glfwGetTime();
            deltaTime        = currentFrame - lastFrame;
            lastFrame        = currentFrame;

            game_renderer.render();

            for (auto& model : game_renderer.model_renderer.static_world_models)
            {
              draw_tree_recursive(tree.root);

              for (const auto& mesh : model.second.draw_model.meshes)
              {
                if (sub_hulls[mesh.VAO].points.empty())
                {
                  sub_hulls[mesh.VAO].load(mesh);
                }
                sub_hulls[mesh.VAO].to_submeshes();
              }
            }

            auto collision_meshes = tree.find(game_renderer.game_camera.vec_position);

            glPointSize(5);

            for (const auto& ite : collision_meshes)
            {
              for (const auto& vert : ite.vertices)
              {
                // debug_overlay_t::draw_point(vert.position, 0xffffffff, false);
              }
            }

            glPointSize(1);

            debug_menu.print_stdcout();
            debug_menu.draw(_window, in_menu, deltaTime);
          })
      .stdexit();
}