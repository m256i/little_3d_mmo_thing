#include <cmath>
#include <deque>

#include <float.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include <stb_image.h>

#include "common.h"
#include "headers/window.h"
#include "headers/base_engine/camera.h"
#include "headers/base_engine/shader.h"
#include "headers/base_engine/model.h"

constinit f32 lastX = 1920.f / 2.0f;
constinit f32 lastY = 1080.f / 2.0f;

static constexpr i64 fps_graph_update_rate = 1; // in seconds
static f32 movement_speed                  = 4;

static bool in_menu           = false;
constinit bool adjusted_mouse = false;
double last_change_time       = 0.0f;

static Camera camera;
static std::deque<float> fps_tracks{};

bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Shader ourShader{};
Model ourModel{};

auto
mouse_callback([[maybe_unused]] GLFWwindow* window, double xpos_, double ypos_) -> u0
{
  if (!in_menu)
  {
    camera.ProcessMouseMovement(-(lastX - xpos_), lastY - ypos_);
    lastX = (f32)xpos_;
    lastY = (f32)ypos_;

    printf("x: %f y: %f\n", lastX, lastY);
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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyboard(UP, movement_speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, movement_speed * deltaTime);
  }
}

auto
main() -> i32
{
  std::cout << "hello!\n";
  return create_window("WoW Clone :D", false)
      .register_callback(glfwSetCursorPosCallback, mouse_callback)
      .init(
          [](GLFWwindow* _window)
          {
            // main loop etc
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();

            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // Setup Platform/Renderer backends
            ImGui_ImplGlfw_InitForOpenGL(_window, true);
            ImGui_ImplOpenGL3_Init("#version 330");

            // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
            stbi_set_flip_vertically_on_load(true);

            // configure global opengl state
            glEnable(GL_DEPTH_TEST | GL_DEPTH_BUFFER_BIT);

            ourShader   = Shader("../1.model_loading.vs", "../1.model_loading.fs");
            ourModel    = Model("../data/valgarde_70gw.obj");
            camera.Zoom = 100;
          })
      .loop(
          [](GLFWwindow* _window)
          {
            // Our state
            bool show_demo_window = true;

            process_input(_window);

            float currentFrame = (float)glfwGetTime();
            deltaTime          = currentFrame - lastFrame;
            lastFrame          = currentFrame;

            // Rendering
            int display_w, display_h;
            glfwGetFramebufferSize(_window, &display_w, &display_h);

            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (!((i64)glfwGetTime() % fps_graph_update_rate))
            {
              static i64 last_update_time = 0;

              if ((i64)glfwGetTime() != last_update_time)
              {
                fps_tracks.push_back(1.f / deltaTime);
                if (fps_tracks.size() >= 20)
                {
                  fps_tracks.pop_front();
                }
                last_update_time = (i64)glfwGetTime();
              }
            }

            // don't forget to enable shader before setting uniforms
            ourShader.use();

            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)display_w / (float)display_h, 0.1f, 1000.0f);
            glm::mat4 view       = camera.GetViewMatrix();

            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);

            // render the loaded model
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
            model           = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            ourShader.setFloat("time", glfwGetTime());

            ourModel.Draw(ourShader);

            if (!in_menu)
            {
              glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            if (in_menu)
            {
              glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            if (in_menu)
            {
              // Start the Dear ImGui frame
              ImGui_ImplOpenGL3_NewFrame();
              ImGui_ImplGlfw_NewFrame();
              ImGui::NewFrame();
              ImGui::ShowDemoWindow(&show_demo_window);
              bool open = true;
              if (ImGui::Begin("perf stats", &open))
              {
                ImGui::Text("fps: %f", 1.f / deltaTime);

                std::vector<float> temp_fps_tracks;
                temp_fps_tracks.reserve(fps_tracks.size());

                for (const auto ite : fps_tracks)
                {
                  temp_fps_tracks.push_back(ite);
                }

                ImGui::PlotLines("fps graph", temp_fps_tracks.data(), temp_fps_tracks.size(), 0, 0, 10, 5000, ImVec2(250, 100));
                ImGui::SliderFloat2("movement speed", &movement_speed, 4, 140);
                ImGui::End();
              }

              ImGui::Render();
            }

            if (in_menu)
            {
              ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            // hello please dont format weirdly
          })
      .stdexit();
}