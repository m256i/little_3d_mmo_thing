#pragma once

#include <common.h>
#include <glad/glad.h>
#include <cassert>

namespace renderer::core::vertex_buffer
{

enum class vbuff_attribute_type
{
  invalid = 0,
  type_f16,
  type_f32,
  type_f64,
  type_i32,
  type_u32,
};

inline vbuff_attribute_type
vbuff_attrib_from_gl(GLenum type)
{
  using e = vbuff_attribute_type;
  // clang-format off
  switch (type)
  {
  case GL_HALF_FLOAT:   return e::type_f16;
  case GL_FLOAT:        return e::type_f32;
  case GL_DOUBLE:       return e::type_f64;
  case GL_INT:          return e::type_i32;
  case GL_UNSIGNED_INT: return e::type_u32;
  }
  return e::invalid;
  // clang-format on
}

inline GLenum
gl_attrib_from_host(vbuff_attribute_type _type)
{
  using e = vbuff_attribute_type;
  // clang-format off
  switch (_type)
  {
  case e::type_f16:  return GL_HALF_FLOAT  ;
  case e::type_f32:  return GL_FLOAT       ;
  case e::type_f64:  return GL_DOUBLE      ;
  case e::type_i32:  return GL_INT         ;
  case e::type_u32:  return GL_UNSIGNED_INT;
  case e::invalid:   return -1;
  }
  return -1;
  // clang-format on
}

struct vertex_buffer_attribute
{
  std::string name;          // name of vertex buffer attribute
  vbuff_attribute_type type; // of of attribute float int, vec3 would be float and count = 3
  usize count;               // count of objects of 'type' float -> 1 vec3 -> 3 mat4 -> 16 etc...

  inline usize
  get_size() const
  {
    usize curr_size = type == vbuff_attribute_type::type_f16 ? sizeof(i16) :
                      type == vbuff_attribute_type::type_f64 ? sizeof(f64) :
                                                               sizeof(f32);
    return count * curr_size;
  }
};

enum class vertex_buffer_type : i32
{
  tris,
  quads,
  lines,
  points
};

namespace detail
{

struct poly_buf
{
  template <typename TType>
  u0
  push_back(const TType& _obj)
  {
    constexpr static usize type_size = sizeof(TType);
    buf.resize(buf.size() + type_size);
    u8* curr_ptr = std::addressof(buf.back()) - sizeof(TType) + 1;
    new (std::launder(curr_ptr)) TType(_obj);
  }

  template <typename TType>
  u0
  push_back(TType&& _obj)
  {
    constexpr static usize type_size = sizeof(TType);
    buf.resize(buf.size() + type_size);
    u8* curr_ptr = std::addressof(buf.back()) - sizeof(TType) + 1;
    new (std::launder(curr_ptr)) TType(std::move(_obj));
  }

  u0
  clear()
  {
    buf.clear();
  }

  u0
  reserve(usize _ns)
  {
    buf.reserve(_ns);
  }

  template <typename TType>
  TType&
  get(usize _index)
  {
    assert(_index + sizeof(TType) < buf.size());
    u8* const raw_ptr = std::addressof(buf.at(_index));
    return *((TType*)(std::launder(raw_ptr)));
  }

  template <typename TType>
  const TType&
  get(usize _index) const
  {
    assert(_index + sizeof(TType) < buf.size());
    const u8* const raw_ptr = std::addressof(buf.at(_index));
    return *((TType*)(std::launder(raw_ptr)));
  }

  usize
  size() const
  {
    return buf.size();
  }

  u8*
  data()
  {
    return buf.data();
  }

  const u8*
  data() const
  {
    return buf.data();
  }

  alignas(max_align_t) std::vector<u8> buf;
};

} // namespace detail

struct vertex_buffer
{
  u32 vao_handle, vbo_handle, ebo_handle;

  constexpr vertex_buffer() = default;

  vertex_buffer(const std::vector<vertex_buffer_attribute>& _attributes) : attributes(_attributes) {}

  u0
  set_attributes(const std::vector<vertex_buffer_attribute>& _new_attributes)
  {
    attributes = _new_attributes;
  }

  u0
  add_attribute(vertex_buffer_attribute _new_attribute)
  {
    assert(!initialized);

    attributes.push_back(_new_attribute);
  }

  u0
  draw()
  {
    if (!initialized)
    {
      return;
    }

    glBindVertexArray(vao_handle);

    switch (buffer_type)
    {
    case vertex_buffer_type::tris:
    {
      glDrawElements(GL_TRIANGLES, (u32)indices.size(), GL_UNSIGNED_INT, nullptr);
      break;
    }
    case vertex_buffer_type::quads:
    {
      glDrawElements(GL_QUADS, (u32)indices.size(), GL_UNSIGNED_INT, nullptr);
      break;
    }
    case vertex_buffer_type::lines:
    {
      glDrawElements(GL_LINES, (u32)indices.size(), GL_UNSIGNED_INT, nullptr);
      break;
    }
    case vertex_buffer_type::points:
    {
      glDrawElements(GL_POINTS, (u32)indices.size(), GL_UNSIGNED_INT, nullptr);
      break;
    }
    }

    assert(glGetError() == GL_NO_ERROR);
  }

  usize
  get_total_attribute_stride()
  {
    usize total = 0;
    for (const auto& ite : attributes)
    {
      usize curr_size = ite.type == vbuff_attribute_type::type_f16 ? 2 : ite.type == vbuff_attribute_type::type_f64 ? 8 : 4;
      total += curr_size * ite.count;
    }
    return total;
  }

  template <typename TType>
  u0
  push_back_vertex(const TType& _obj)
  {
    assert(sizeof(TType) == get_total_attribute_stride());
    raw_buffer.push_back<TType>(_obj);
  }

  template <typename TType>
  TType&
  get_vertex(usize _vertex_byte_index)
  {
    return raw_buffer.get<TType>(_vertex_byte_index);
  }

  template <typename TType>
  const TType&
  get_vertex(usize _vertex_byte_index) const
  {
    return raw_buffer.get<TType>(_vertex_byte_index);
  }

  u0
  push_back_index(u32 _index)
  {
    indices.push_back(_index);
  }

  u0
  initialize_with_data()
  {
    assert(!initialized);

    glGenVertexArrays(1, &vao_handle);
    glGenBuffers(1, &vbo_handle);
    glGenBuffers(1, &ebo_handle);

    glBindVertexArray(vao_handle);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, raw_buffer.size(), raw_buffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    const usize attrib_stride = get_total_attribute_stride();
    usize current_offset      = 0;

    usize attrib_index = 0;
    for (const auto& vertex_attribute : attributes)
    {
      printf("binding vertex attrib: index %d, count %d, type: %d, stride: %d, offset: %d", attrib_index, vertex_attribute.count,
             gl_attrib_from_host(vertex_attribute.type), attrib_stride, current_offset);

      glEnableVertexAttribArray(attrib_index);
      glVertexAttribPointer(attrib_index, vertex_attribute.count, gl_attrib_from_host(vertex_attribute.type), GL_FALSE, attrib_stride,
                            (u0*)current_offset);

      assert(glGetError() == GL_NO_ERROR);

      current_offset += vertex_attribute.get_size();
      attrib_index++;
    }
    glBindVertexArray(0);
    assert(glGetError() == GL_NO_ERROR);
    initialized = true;
  }

  u0
  destroy()
  {
    glDeleteVertexArrays(1, &vao_handle);
    glDeleteBuffers(1, &vbo_handle);
    glDeleteBuffers(1, &ebo_handle);

    raw_buffer.clear();
    indices.clear();
    attributes.clear();

    initialized = false;
  }

  u0
  clear_buffers()
  {
    raw_buffer.clear();
    indices.clear();
  }

  u0
  reinitialize_with_data()
  {
    if (!initialized)
    {
      LOG(INFO) << "reinitializing uninitialized vertex buffer";
      initialize_with_data();
      return;
    }

    glDeleteVertexArrays(1, &vao_handle);
    glDeleteBuffers(1, &vbo_handle);
    glDeleteBuffers(1, &ebo_handle);

    initialized = false;

    initialize_with_data();
  }

  detail::poly_buf raw_buffer;
  std::vector<u32> indices{};
  std::vector<vertex_buffer_attribute> attributes{};
  vertex_buffer_type buffer_type{vertex_buffer_type::tris};
  bool initialized{false};
};

} // namespace renderer::core::vertex_buffer
