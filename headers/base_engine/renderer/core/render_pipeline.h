#pragma once
#include <common.h>

#include "vertex_buffer.h"
#include "../shader.h"
#include "image_texture.h"
#include "frame_buffer.h"

#include <base_engine/utils/fnv1a.h>

#include <vector>
#include <variant>

#include <magic_enum/magic_enum.hpp>

namespace renderer::core::pipeline
{

struct render_pipeline_texture_input
{
  const char *name, *path;
};

struct render_pipeline_shader_input
{
  const char *name, *vert_path, *frag_path;
};

inline vertex_buffer::vertex_buffer_attribute
shader_to_vbuf_attrib(const shader_attribute& _sattrib)
{
  vertex_buffer::vertex_buffer_attribute out;
  out.name = _sattrib.name;

  using e = shader_attribute_type;

  // clang-format off
  switch (_sattrib.type) {
  case  e::type_f32:    out.type = vertex_buffer::vbuff_attribute_type::type_f32;   out.count = 1;  break;
  case  e::type_f64:    out.type = vertex_buffer::vbuff_attribute_type::type_f64;   out.count = 1;  break;
  case  e::type_i32:    out.type = vertex_buffer::vbuff_attribute_type::type_i32;   out.count = 1;  break;
  case  e::type_u32:    out.type = vertex_buffer::vbuff_attribute_type::type_u32;   out.count = 1;  break;
  case  e::type_vec2i:  out.type = vertex_buffer::vbuff_attribute_type::type_i32;   out.count = 2;  break;
  case  e::type_vec2f:  out.type = vertex_buffer::vbuff_attribute_type::type_f32;   out.count = 2;  break;
  case  e::type_vec3i:  out.type = vertex_buffer::vbuff_attribute_type::type_i32;   out.count = 3;  break;
  case  e::type_vec3f:  out.type = vertex_buffer::vbuff_attribute_type::type_f32;   out.count = 3;  break;
  case  e::type_vec4i:  out.type = vertex_buffer::vbuff_attribute_type::type_i32;   out.count = 4;  break;
  case  e::type_vec4f:  out.type = vertex_buffer::vbuff_attribute_type::type_f32;   out.count = 4;  break;
  case  e::type_mat2:   out.type = vertex_buffer::vbuff_attribute_type::type_f32;   out.count = 4;  break;
  case  e::type_mat3:   out.type = vertex_buffer::vbuff_attribute_type::type_f32;   out.count = 9;  break;
  case  e::type_mat4:   out.type = vertex_buffer::vbuff_attribute_type::type_f32;   out.count = 16; break;
  default: {
    out.type = vertex_buffer::vbuff_attribute_type::invalid;
    out.count = -1;
  }
  }
  // clang-format on
  return out;
}

inline bool
operator==(const shader_attribute& _lhs, const vertex_buffer::vertex_buffer_attribute& _rhs)
{
  auto temp = shader_to_vbuf_attrib(_lhs);

  puts("operator==");
  printf("temp vertex type: %s\n", magic_enum::enum_name(temp.type).data());
  printf("temp vertex count: %d\n", temp.count);

  if (temp.type == vertex_buffer::vbuff_attribute_type::invalid)
  {
    return false;
  }
  return temp.count == _rhs.count && temp.name == _rhs.name && temp.type == _rhs.type;
}

/*
takes vertex buffers, input textures, shaders. makes sure they match and then generates a stack allocated array of function pointers that
can be called as a draw call
*/
struct render_pipeline
{
  renderer::core::vertex_buffer::vertex_buffer vbuf;
  std::vector<render_pipeline_texture_input> textures;
  render_pipeline_shader_input shader;
  std::optional<gl_fbo_texture> fbo;

  using uniform_callback_return_type = std::variant<f32, f64, u32, i32, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

  u0
  load_texture_from_path(const char* _tex_name, const char* _tex_path)
  {
    assert(texture_cache.contains(fnv1a::hash(_tex_name)));
    texture_cache[fnv1a::hash(_tex_name)].load(_tex_path);
  }

  u0
  initialize()
  {
    /*
    check if vbud matches shader
    */
    for (const auto& tex : textures)
    {
      if (texture_cache.contains(fnv1a::hash(tex.name)))
      {
        LOG(INFO) << "pipeline init texture: " << tex.name << " already in cache!";
        continue;
      }

      if (tex.name && tex.path)
      {
        texture_cache[fnv1a::hash(tex.name)] = image_tex_lod(tex.name, tex.path);
        texture_cache[fnv1a::hash(tex.name)].load(tex.path, tex.name);
      }
      else if (tex.name)
      {
        texture_cache[fnv1a::hash(tex.name)] = image_tex_lod(tex.name, "");
      }
      else
      {
        assert(false);
      }
    }

    LOG(INFO) << "loading shader: ";

    assert(internal_shader.load_from_path(shader.vert_path, shader.frag_path));

    const auto input_attributes = internal_shader.get_input_attributes();

    printf("shader inputs list length: %d\n", input_attributes.size());

    for (const auto& vertex_attrib : vbuf.attributes)
    {
      printf("checking vertex attribute: %s length %d\n", vertex_attrib.name.data(), vertex_attrib.name.length());
      bool found = false;
      for (const auto& shader_attrib : input_attributes)
      {
        printf("against shader attribute: %s length: %d\n", shader_attrib.name.data(), shader_attrib.name.length());

        if (shader_attrib.name == vertex_attrib.name)
        {
          puts("name same");
          printf("shade attrib type: %s\n", magic_enum::enum_name(shader_attrib.type).data());
          printf("vertex attrib type: %s count: %d\n", magic_enum::enum_name(vertex_attrib.type).data(), vertex_attrib.count);
          if (shader_attrib == vertex_attrib)
          {
            puts("same type");
            found = true;
          }
        }
      }
      if (!found)
      {
        LOG(DEBUG) << "vertex attribute and shader input attribute mismatch!";
        assert(false);
      }
    }

    for (const auto& uniform : internal_shader.get_uniforms())
    {
      uniform_names.push_back(fnv1a::hash(uniform.name.c_str()));
    }

    const auto shader_textures = internal_shader.get_input_textures();

    for (const auto& shader_ipt_texture : shader_textures)
    {
      bool found = false;
      for (const auto& ipt_texture : texture_cache)
      {
        printf("checking sipt %s against %s: ", shader_ipt_texture.data(), ipt_texture.second.name.data());
        if (ipt_texture.second.name == shader_ipt_texture)
        {
          found = true;
        }
      }

      if (!found)
      {
        LOG(DEBUG) << shader_ipt_texture << " texture not provided in c++ code";
        assert(false);
      }
    }

    draw_function = [&]()
    {
      internal_shader.use();

      for (const auto& uniform_name : uniform_names)
      {
        /*
        TODO: hashes are a thing
        */
        const auto uniform_name_hash = uniform_name;

        if (uniform_callbacks.contains(uniform_name_hash))
        {
          const auto value = uniform_callbacks[uniform_name_hash]();
          switch (value.index())
          {
          // uniform_callback_return_etype::type_f32: shader.setFloat(uniform_name, value.)break;
          case 0:
            internal_shader.setFloat(uniform_name_hash, std::get<f32>(value));
            break;
          // uniform_callback_return_etype::type_f64: break;
          case 1:
            // TODO: implement
            assert(false);
            break;
          // uniform_callback_return_etype::type_u32
          case 2:
            assert(false);
            break;
          // uniform_callback_return_etype::type_i32
          case 3:
            internal_shader.setInt(uniform_name_hash, std::get<i32>(value));
            break;
          // uniform_callback_return_etype::type_vec2
          case 4:
            internal_shader.setVec2(uniform_name_hash, std::get<glm::vec2>(value));
            break;
          // uniform_callback_return_etype::type_vec3
          case 5:
            internal_shader.setVec3(uniform_name_hash, std::get<glm::vec3>(value));
            break;
          // uniform_callback_return_etype::type_vec4
          case 6:
            internal_shader.setVec4(uniform_name_hash, std::get<glm::vec4>(value));
            break;
          // uniform_callback_return_etype::type_mat3
          case 7:
            internal_shader.setMat3(uniform_name_hash, std::get<glm::mat3>(value));
            break;
            // uniform_callback_return_etype::type_mat4
          case 8:
            internal_shader.setMat4(uniform_name_hash, std::get<glm::mat4>(value));
            break;
          }
        }
      }

      u32 texidx = 0;
      for (const auto& shader_ipt_texture : texture_cache)
      {
        glActiveTexture(GL_TEXTURE0 + (GLenum)texidx);
        assert(glGetError() == GL_NO_ERROR);
        internal_shader.setInt(shader_ipt_texture.first, texidx);
        assert(glGetError() == GL_NO_ERROR);
        shader_ipt_texture.second.bind();
        assert(glGetError() == GL_NO_ERROR);
        texidx++;
      }

      if (fbo.has_value())
      {
        fbo->bind_frame_buffer();
      }
      else
      {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }

      vbuf.draw();
    };

    LOG(INFO) << "initialized pipeline!";

    initialized = true;
  }

  u0
  load_texture_from_memory(const char* _texture_name, u8* _tex_buf, usize _size_x, usize _size_y, i32 _channels)
  {
    texture_cache[fnv1a::hash(_texture_name)].destroy();
    texture_cache[fnv1a::hash(_texture_name)].load_from_mem(_tex_buf, _size_x, _size_y, _channels);
    texture_cache[fnv1a::hash(_texture_name)].name = _texture_name;

    LOG(INFO) << "texture added to cache: " << _texture_name;
  }

  u0
  set_instance_count(u32 _instance_count)
  {
    vbuf.set_instance_count(_instance_count);
  }

  u32
  get_instance_count()
  {
    return vbuf.get_instance_count();
  }

  u0
  set_base_instance_index(usize _idx)
  {
    vbuf.base_instance_index = _idx;
  }

  u0
  clear_buffers()
  {
    vbuf.clear_buffers();
  }

  template <typename TType>
  TType&
  get_vertex(usize _vertex_buffer_byte_index)
  {
    vbuf.get_vertex<TType>(_vertex_buffer_byte_index);
  }

  template <typename TType>
  const TType&
  get_vertex(usize _vertex_buffer_byte_index) const
  {
    vbuf.get_vertex<TType>(_vertex_buffer_byte_index);
  }

  template <typename TType>
  u0
  push_back_vertex(const TType& _obj)
  {
    vbuf.push_back_vertex(_obj);
  }

  template <typename TType>
  u0
  push_back_vertex(TType&& _obj)
  {
    vbuf.push_back_vertex(_obj);
  }

  u0
  push_back_index(u32 _index)
  {
    vbuf.push_back_index(_index);
  }

  u0
  add_uniform_callack(std::string_view _uniform_name, const std::function<uniform_callback_return_type()>& _callback)
  {
    u64 hash                = fnv1a::hash(std::string(_uniform_name).c_str());
    uniform_callbacks[hash] = _callback;
  }

  u0
  setup_drawbuffer()
  {
    if (!vbuf.initialized)
    {
      vbuf.initialize_with_data();
    }
    else
    {
      vbuf.reinitialize_with_data();
    }
  }

  /*
  TODO: possible micro-optimization: all of these things can be allocated on stack and use fast
  */
  std::vector<usize> uniform_names{};
  std::unordered_map<u64, std::function<uniform_callback_return_type()>> uniform_callbacks{};
  std::unordered_map<u64, image_tex_lod> texture_cache{};
  basic_shader_t internal_shader{};
  std::function<void()> draw_function{};
  bool initialized = false;
};

} // namespace renderer::core::pipeline