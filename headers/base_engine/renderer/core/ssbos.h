#pragma once

#include <cassert>
#include <common.h>
#include <glad/glad.h>
#include <GL/gl.h>

namespace core
{
struct ssbo_t
{
  ssbo_t(usize _binding_point) : binding_point(_binding_point) {}

  u0
  set_size(usize _new_byte_size)
  {
    if (_new_byte_size == current_size)
    {
      return;
    }

    if (ssbo_handle)
    {
      glDeleteBuffers(1, &ssbo_handle);
    }

    glGenBuffers(1, &ssbo_handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _new_byte_size, nullptr, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, ssbo_handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    current_size = _new_byte_size;

    if (!ssbo_handle)
    {
      assert(false);
    }
  }

  u0
  buffer_data(u8 *_data, usize _data_size)
  {
    assert(current_size != 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_handle);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _data_size, _data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  u0
  bind()
  {
    assert(ssbo_handle != 0);
    assert(current_size != 0);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, ssbo_handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_handle);
  }

  u0
  unbind()
  {
    assert(ssbo_handle != 0);
    assert(current_size != 0);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  GLuint ssbo_handle{};
  usize current_size{0};
  const usize binding_point;
};
} // namespace core
