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
#include "base_engine/renderer/static_world_model.h"
#include "common.h"

#include "assimp/aabb.h"
#include "assimp/vector3.h"
#include "common.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "headers/base_engine/renderer/core/lod.h"
#include "headers/base_engine/renderer/model.h"
#include "headers/base_engine/renderer/model_renderer.h"
#include "headers/base_engine/world_generation/ground_chunking.h"
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

#include <base_engine/world_generation/ground_mesh.h>

#include <base_engine/renderer/instanced_model.h>
#include <base_engine/renderer/core/frame_buffer.h>

#include <base_engine/world_generation/ground_chunking.h>
#include <base_engine/world_generation/ground_chunk.h>
#include <base_engine/scripting/scripting_api.h>

#include <base_engine/renderer/static_render_model.h>

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
      in_menu          = !in_menu;
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

  renderer::instanced_render_model_lod instanced_grass;

  // lod_static_world_model_t LOD_tree{"lod_test_tree"};

  // ground_mesh_chunk_t plane{};
  // ground_mesh_system world{};
  world_gen::ground_chunk ground;

  // shader_thing.get_uniform<"model", glm::mat4>()[0][0] = 69;
  // std::cout << "ASDHASDH: " << shader_thing.get_uniform<"model", glm::mat4>()[0][0] << "\n";

  renderer::core::base_texture2d tex1, tex2;
  renderer::image_tex_lod asdasd;

  renderer::static_render_model TREEYes;

  asdasd.set_lod_level(lod::detail_level::lod_detail_mid);

  puts("bruder was");

  // auto pipeline = renderer::core::combine{buffer | tex1 | asdasd} > shader_thing > framebuftest;

  puts("what");

  static_world_model_t skybox3d_model{"skybox"};

  pp_fs_texture post_processor;

  usize pp_pass1;

  bool wireframe_view = false;

  return create_window("WoW Clone :D", false)
      .register_callback(glfwSetCursorPosCallback, mouse_callback)
      .init(
          [&](GLFWwindow* _window)
          {
            debug_menu.init_menu(_window);

            debug_menu.add_debug_widget("terrain", debug_menu_t::debug_widget_type::button, "clear noise handles",
                                        [&](f64 val)
                                        {
                                          scripting::noise_impl::noise_handles.clear();
                                          while (!scripting::noise_impl::returned_handles.empty())
                                          {
                                            scripting::noise_impl::returned_handles.pop();
                                          }
                                          scripting::noise_impl::last_handle = 1;
                                        });

            debug_menu.add_user_script_module("terrain", "../scripts/terrain.wren", "terrain", "terrain_value(_,_,_)",
                                              "needs class 'terrain' with method 'terrain_value(x,y,z) -> double'");

            debug_menu.add_user_script_module("texturing", "../scripts/texturing.wren", "texturer", "texture_value(_,_,_)",
                                              "needs class 'texturer' with method 'texture_value(x,y,z) -> double'");

            debug_menu.add_lib_script_module("noiselib", "../scripts/corelib/noiselib.wren", "API implementation of noise functions");
            debug_menu.add_lib_script_module("dbglib", "../scripts/corelib/dbglib.wren", "API implementation of debug functions");

            debug_menu.add_debug_widget("model renderer", debug_menu_t::debug_widget_type::slider_f32, "wireframe",
                                        [&](double val)
                                        {
                                          if (val >= 0.0)
                                          {
                                            wireframe_view = true;
                                          }
                                          else
                                          {
                                            wireframe_view = false;
                                          }
                                        });

            debug_overlay_t::init(_window, game_renderer.game_camera);

            game_renderer.update_frame_buffer(_window);

            post_processor.initialize(1920, 1080); // @FIXME: cleanup
            pp_pass1 = post_processor.add_post_pass(
                R"(
#version 430 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texture_diffuse;

void main()
{
  vec3 col =vec3(1,1,1) - texture(texture_diffuse, TexCoords).rgb;
  //float avg = (col.r + col.g + col.b) / 3;
 // FragColor = vec4(1-avg,1-avg,1-avg, 1);
  FragColor = vec4(col, 1);
}

)");

            // plane.load_shader();
            // plane.initialize(0);

            // world.init(debug_menu);
            ground.init(10'000, debug_menu);

            game_renderer.model_renderer.add_static_world_model("tree1", "../data/trees/trees/westfalltree03.obj");

            LOG(INFO) << "retarr";

            instanced_grass.load_model_from_file("../data/trees/trees/9ard_ardenweald_largetree04.obj");

            LOG(INFO) << "loaded stupid test grass";
            // instanced_model.init_shader("../basic_model_instanced.vs", "../basic_model_instanced.fs");

            // TREEYes.load_from_file("../data/playe_models/D0510041.obj");
            TREEYes.load_from_file("../data/trees/trees/9ard_ardenweald_largetree04.obj");
            TREEYes.update_location();

            skybox3d_model.load_model("../data/sky/3d/dalaranskybox.obj");
            skybox3d_model.init_shader("../3d_sky_box.vs", "../3d_sky_box.fs");

            static constexpr int county = 10;

            instanced_grass.set_instance_count(county * county);

            for (usize i = 0; i < county * county; ++i)
            {
              auto& instance_data          = instanced_grass.get_instance_data()[i];
              instance_data.world_position = {(i % county) * 1500.f, -107.f, (i / county) * 1500.f};
              instance_data.world_scale    = {15, 15, 15};
              instanced_grass.apply_translation_at(i);
            }
            instanced_grass.buffer();

            instanced_grass.initialize_culling_grid();

            // LOD_tree.load_model("../data/trees/trees/9ard_ardenweald_largetree04.obj");
            // LOD_tree.init_shader();

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
            // glPatchParameteri(GL_PATCH_VERTICES, 4);
          })
      .loop(
          [&](GLFWwindow* _window)
          {
            glClearColor(0, 0, 0, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            process_input(_window);

            post_processor.update_size(1920, 1080);

            f32 currentFrame = (f32)glfwGetTime();
            deltaTime        = currentFrame - lastFrame;
            lastFrame        = currentFrame;

            f64 cX, cY;
            glfwGetCursorPos(_window, &cX, &cY);

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

            glm::mat4 projection = glm::perspective(glm::radians(game_renderer.game_camera.fov), 1920.f / 1080.f, 0.1f, 10'000.f);
            glm::mat4 view       = game_renderer.game_camera.get_view_matrix();

            skybox3d_model.vec_position = game_renderer.game_camera.vec_position;
            skybox3d_model.vec_rotation = glm::vec3{0, (f32)glfwGetTime() / 40, 0};

            const auto renderings = [&]() {};

            skybox3d_model.draw(projection, view);
            glClear(GL_DEPTH_BUFFER_BIT);

            // plane.world_position = glm::vec3{0, 0, 0};
            // plane.draw(game_renderer.display_w, game_renderer.display_h, game_renderer.game_camera, lod::detail_level::lod_detail_potato,
            //            0xffffffff);

            // ground.draw(projection, view, game_renderer.display_w, game_renderer.display_h, game_renderer.game_camera);
            if (wireframe_view)
            {
              glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }

            instanced_grass.draw(game_renderer.game_camera);

            if (wireframe_view)
            {
              glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            // TREEYes.rotation = {std::sin(glfwGetTime()) * 2, std::cos(glfwGetTime()) * 2, 0};
            // TREEYes.scale    = {(std::sin(3.14194732 + glfwGetTime()) + 1) * 0.5, (std::sin(glfwGetTime()) + 1) * 0.5,
            //                     (std::cos(glfwGetTime()) + 1) * 0.5};
            // TREEYes.position = {std::sin(glfwGetTime()) * 2, 0, std::cos(glfwGetTime()) * 2};

            // TREEYes.update_location();

            // TREEYes.draw(game_renderer.game_camera);
            // TREEYes.translated_bbox.debug_draw_axes();

            // post_processor.bake(pp_pass1, renderings);
            // post_processor.draw(pp_pass1);

            debug_menu.print_stdcout();
            debug_menu.draw(_window, in_menu, deltaTime);
            // game_renderer.render();
          })
      .stdexit();
}