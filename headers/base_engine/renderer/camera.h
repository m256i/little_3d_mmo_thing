#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct frust_plane
{
  // unit vector
  glm::vec3 normal = {0.f, 1.f, 0.f};

  // distance from origin to the nearest point in the plane
  float distance = 0.f;

  frust_plane(const glm::vec3& point, const glm::vec3& normalVec)
  {
    normal   = glm::normalize(normalVec); // Ensure the normal is a unit vector
    distance = glm::dot(normal, point);   // Compute distance from origin to the plane
  }

  float
  signed_dist_to_center(const glm::vec3& point) const
  {
    return glm::dot(normal, point) - distance;
  }

  frust_plane() = default;
};

struct camera_frust
{
  frust_plane topFace;
  frust_plane bottomFace;

  frust_plane rightFace;
  frust_plane leftFace;

  frust_plane farFace;
  frust_plane nearFace;
};

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
  constexpr static float default_speed       = 6.5f;
  constexpr static float default_sensitivity = 0.1f;
  constexpr static float default_fov         = 90.0f;

  glm::vec3 vec_position, vec_front, vec_up, vec_right, vec_world_up;

  float yaw, pitch;
  float movement_speed, mouse_sense, fov;

  render_camera_t(glm::vec3 _vec_position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f),
                  float _yaw = default_yaw, float _pitch = default_pitch)
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

  camera_frust
  createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar) const
  {
    camera_frust frustum;
    const float halfVSide        = zFar * tanf(fovY * .5f);
    const float halfHSide        = halfVSide * aspect;
    const glm::vec3 frontMultFar = zFar * vec_front;

    frustum.nearFace   = frust_plane{vec_position + zNear * vec_front, vec_front};
    frustum.farFace    = frust_plane{vec_position + frontMultFar, -vec_front};
    frustum.rightFace  = frust_plane{vec_position, glm::cross(frontMultFar - vec_right * halfHSide, vec_up)};
    frustum.leftFace   = frust_plane{vec_position, glm::cross(vec_up, frontMultFar + vec_right * halfHSide)};
    frustum.topFace    = frust_plane{vec_position, glm::cross(vec_right, frontMultFar - vec_up * halfVSide)};
    frustum.bottomFace = frust_plane{vec_position, glm::cross(frontMultFar + vec_up * halfVSide, vec_right)};

    return frustum;
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
    vec_right = glm::normalize(glm::cross(vec_front, vec_world_up)); // normalize the vectors, because their length gets closer to 0 the
                                                                     // more you look up or down which results in slower movement.
    vec_up = glm::normalize(glm::cross(vec_right, vec_front));
  }
};