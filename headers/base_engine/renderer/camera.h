#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct render_camera_t
{
  enum class e_movement_types
  {
    forward,
    backward,
    left,
    right,
    up,
    down
  };

  constexpr static float default_yaw         = -90.0f;
  constexpr static float default_pitch       = 0.0f;
  constexpr static float default_speed       = 15.5f;
  constexpr static float default_sensitivity = 0.1f;
  constexpr static float default_fov         = 90.0f;

  glm::vec3 vec_position, vec_front, vec_up, vec_right, vec_world_up;

  float yaw, pitch;
  float movement_speed, mouse_sense, fov;

  render_camera_t(glm::vec3 _vec_position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = default_yaw, float _pitch = default_pitch)
      : vec_front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(default_speed), mouse_sense(default_sensitivity), fov(default_fov)
  {
    vec_position = _vec_position;
    vec_world_up = _up;
    yaw          = _yaw;
    pitch        = _pitch;

    update_vectors();
  }

  glm::mat4
  get_view_matrix() const
  {
    return glm::lookAt(vec_position, vec_position + vec_front, vec_up);
  }

  void
  process_keyboard_input(e_movement_types direction, float _delta_time)
  {
    float velocity = movement_speed * _delta_time;
    if (direction == e_movement_types::forward) vec_position += vec_front * velocity;
    if (direction == e_movement_types::backward) vec_position -= vec_front * velocity;
    if (direction == e_movement_types::left) vec_position -= vec_right * velocity;
    if (direction == e_movement_types::right) vec_position += vec_right * velocity;
    if (direction == e_movement_types::up) vec_position.y += velocity;
    if (direction == e_movement_types::down) vec_position.y -= velocity;
  }

  void
  process_mouse_movement(float xoffset, float yoffset, bool constrainpitch = true)
  {
    xoffset *= mouse_sense;
    yoffset *= mouse_sense;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainpitch)
    {
      if (pitch > 89.0f) pitch = 89.0f;
      if (pitch < -89.0f) pitch = -89.0f;
    }

    update_vectors();
  }

  void
  update_vectors()
  {
    glm::vec3 front;
    front.x   = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y   = sin(glm::radians(pitch));
    front.z   = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    vec_front = glm::normalize(front);
    // also re-calculate the vec_right and vec_up vector
    vec_right =
        glm::normalize(glm::cross(vec_front, vec_world_up)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    vec_up = glm::normalize(glm::cross(vec_right, vec_front));
  }
};