#include <cmath>
#include <deque>

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

#include "assimp/aabb.h"
#include "assimp/vector3.h"
#include "common.h"
#include "headers/base_engine/renderer/model_renderer.h"
#include "headers/window.h"

#include "headers/base_engine/physics_system/bbox.h"
#include "headers/base_engine/renderer/shader.h"

#include "headers/base_engine/renderer/game_renderer.h"
#include "headers/base_engine/physics_system/mesh_collider.h"

#include "headers/base_engine/debug/debug_menu.h"
#include "headers/base_engine/debug/debug_overlay.h"

#define ELPP_DEFAULT_LOG_FILE "wow_clone"

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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) game_renderer.game_camera.process_keyboard_input(mt::forward, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) game_renderer.game_camera.process_keyboard_input(mt::backward, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) game_renderer.game_camera.process_keyboard_input(mt::left, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) game_renderer.game_camera.process_keyboard_input(mt::right, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) game_renderer.game_camera.process_keyboard_input(mt::up, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) game_renderer.game_camera.process_keyboard_input(mt::down, movement_speed * deltaTime);
  }
}

/* TODO:
remove stupi monad thing
input system

*/

INITIALIZE_EASYLOGGINGPP

auto
main(i32 argc, char** argv) -> i32
{
  START_EASYLOGGINGPP(argc, argv);

  std::unordered_map<u32, debug_bbox_t> debug_boxes{};
  debug_menu_t debug_menu{};
  partial_spacial_tree_t tree{};
  debug_overlay_t doverlay{};

  return create_window("WoW Clone :D", false)
      .register_callback(glfwSetCursorPosCallback, mouse_callback)
      .init(
          [&](GLFWwindow* _window)
          {
            debug_menu.init_menu(_window);
            doverlay.init(_window, game_renderer.game_camera);

            game_renderer.update_frame_buffer(_window);
            game_renderer.model_renderer.add_model("dungeon", "../data/valgarde_70gw.obj");

            glEnable(GL_BLEND);
          })
      .loop(
          [&](GLFWwindow* _window)
          {
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            process_input(_window);

            float currentFrame = (float)glfwGetTime();
            deltaTime          = currentFrame - lastFrame;
            lastFrame          = currentFrame;

            game_renderer.render();

            for (auto& model : game_renderer.model_renderer.static_world_models)
            {
              static constexpr auto toVec3 = [](const aiVector3D& _vec) { return glm::vec3{_vec.x, _vec.y, _vec.z}; };

              for (auto& mesh : model.second.draw_model.meshes)
              {
                doverlay.draw_AABB(toVec3(mesh.bbox.mMin), toVec3(mesh.bbox.mMax), 0x0ff4f0ff, true);
              }

              static auto bla = tree.generate(model.second.draw_model.meshes);
              for (const auto& blabal : bla)
              {
                doverlay.draw_AABB(blabal.min, blabal.max, (u32)rand() | 0xff0000ff, true);
              }
            }

            debug_menu.print_stdcout();
            debug_menu.draw(_window, in_menu, deltaTime);
          })
      .stdexit();
}