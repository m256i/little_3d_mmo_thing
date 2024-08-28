#pragma once

#include <common.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <cassert>
#include "pipeline.h"
#include "shader.h"

#include "gl_type_translations.h"

namespace renderer::core
{

template <ct_string TName, typename... TTypes>
struct drawbuf_attrib
{
  template <typename T>
  using rall_t = typename std::remove_cvref_t<std::remove_all_extents_t<T>>;

  template <typename T, typename Tuple>
  struct is_type_in_tuple;

  template <typename T, typename... Ts>
  struct is_type_in_tuple<T, std::tuple<Ts...>> : std::disjunction<std::is_same<T, Ts>...>
  {
  };

  static_assert((is_type_in_tuple<rall_t<TTypes>, allowed_types>::value && ...), "trying to use an invalid type in drawbuffer attribute");
  static_assert((std::is_same_v<nth_type<0, TTypes...>, TTypes> && ...), "attribute types must all be the same");

  std::tuple<TTypes...> attribute_tuple;
  using attribute_common_type = nth_type<0, TTypes...>;

  constexpr static auto raw_name          = TName;
  constexpr static usize attribute_size   = (sizeof(TTypes) + ...);
  constexpr static usize attribute_length = sizeof...(TTypes);
  constexpr static std::string_view attribute_name{TName.to_view()};
};

enum class drawbuffer_type
{
  tris,
  quads,
  tri_strips,
  lines,
  points
};

/*
the class that does the thing!
*/
template <drawbuffer_type TDrawBufType = drawbuffer_type::tris, typename... TAttributes>
struct static_drawbuffer
{
private:
  template <typename T>
  struct is_drawbuf_attrib : std::false_type
  {
  };

  template <ct_string TName, typename... TTypes>
  struct is_drawbuf_attrib<drawbuf_attrib<TName, TTypes...>> : std::true_type
  {
  };

  template <typename T>
  inline static constexpr bool is_drawbuf_attrib_v = is_drawbuf_attrib<T>::value;

public:
  static_assert((is_drawbuf_attrib_v<TAttributes> && ...), "all parameters must be instantiations of drawbuf_attrib.");

  /*
  ordering sequential! always!
  */

  /* used to see which drawbuffers can be combined later */
  constexpr static auto drawbuffer_type = TDrawBufType;
  constexpr static std::tuple<TAttributes...> attribute_row;

  std::vector<u8> vertex_data_cpu_buffer{};
  std::vector<i32> cpu_index_buffer{};

  bool initialized = false;

  u32 vao_handle{}, vbo_handle{}, ebo_handle{};

  template <typename TInAttibuteTuple>
  static consteval bool
  shader_matches_drawbuf()
  {
    bool found_faulty = false;

    for_constexpr<0, sizeof...(TAttributes), 1>(
        [&](auto ite)
        {
          if (found_faulty)
          {
            return;
          }

          constexpr auto vertex_attrib = std::get<ite>(attribute_row);
          constexpr auto shader_ipt    = std::get<ite>(TInAttibuteTuple{});

          if (shader_ipt.name != vertex_attrib.attribute_name)
          {
            found_faulty = true;
          }
          if (!std::is_same_v<typename decltype(shader_ipt)::type, typename decltype(vertex_attrib)::attribute_common_type>)
          {
            found_faulty = true;
          }
        });

    return found_faulty;
  }

  template <typename TInAttibuteTuple>
  vertex_buffer_pipeline_stage
  operator>(shader_pipeline_stage<TInAttibuteTuple> _shader)
  {
    static_assert(std::tuple_size_v<TInAttibuteTuple> == sizeof...(TAttributes),
                  "mismatch of vertex buffer attributes and shader input attributes");

    /*
    test attributes against shader inputs
    */
    static_assert(!shader_matches_drawbuf<TInAttibuteTuple>(), "mismatch in vertex attributes and shader inputs!");

    _shader.pipeline_stage.push_back(
        [&]()
        {
          puts("binding vertex buffer");
          bind_buffers();
        });
    return {std::move(_shader.pipeline_stage)};
  }

  bool
  buffer_to_gpu()
  {
    if (initialized)
    {
      return false;
    }

    assert(vertex_data_cpu_buffer.size() / sizeof(attribute_row) == cpu_index_buffer.size());

    glGenVertexArrays(1, &vao_handle);
    glGenBuffers(1, &vbo_handle);
    glGenBuffers(1, &ebo_handle);

    /*
    buffer the vert data
    */
    glBindVertexArray(vao_handle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_cpu_buffer.size(), vertex_data_cpu_buffer.data(), GL_STATIC_DRAW);

    /*
    buffer the indices
    */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cpu_index_buffer.size() * sizeof(i32), cpu_index_buffer.data(), GL_STATIC_DRAW);

    for_constexpr<0, sizeof...(TAttributes), 1>(
        [&](auto ite)
        {
          /*
          im saying attribute row here a lot but i just mean the "structure" that we send to the gpu
          like for example we're basically sending the gpu a vector of
          struct {
            vec3 position, normal;
          }
          structures and one "row" is just one instantiation of that structure
          */
          using attrib_type = nth_type<ite, TAttributes...>;
          glVertexAttribPointer(ite,
                                /*
                                this is the amount of primitives (floats/ints/uints) in this attribute
                                for example a mat4 is just 16 floats
                                  so attrib_type::attribute_length = 1 means we have a single mat4 as an attribute
                                  glsl_type_translation<>::type_count is how many floats/ints/uints there are in one mat4
                                    so in the case of mat4 16
                                  so this argument is 16
                                */
                                attrib_type::attribute_length *
                                    glsl_type_translation<typename attrib_type::attribute_common_type>::type_count,
                                /*
                                glsl_type_translation<>::gl_type gives you GL_FLOAT, GL_UNSIGNED_INT, GL_INT
                                based on what is actually contained in the datatype vec3 -> f32 vec3i -> i32 etc...
                                */
                                glsl_type_translation<typename attrib_type::attribute_common_type>::gl_type,
                                /* not using fixed size precision on cpu */
                                GL_FALSE,
                                /* stride for the gpu is just the size of one attribute "row" */
                                sizeof(attribute_row),
                                /*
                                this is the offset of the current attribute in the attribute "row"
                                */
                                (u0 *)get_attribte_offset<attrib_type::raw_name>());
          glEnableVertexAttribArray(ite);
        });

    /* reset */
    glBindVertexArray(0);

    initialized = true;
    return true;
  }

  bool
  set_buffers(u0 *_vertex_buf, usize _vertex_buf_len, const std::span<i32> &_idx_buffer)
  {
    vertex_data_cpu_buffer.resize(_vertex_buf_len);
    /* @FIXME: in the future replace with hardware specific AVX2/AVX512 memcpy version */
    std::memcpy(vertex_data_cpu_buffer.data(), _vertex_buf, _vertex_buf_len);

    cpu_index_buffer.resize(_idx_buffer.size());
    /* @FIXME: in the future replace with hardware specific AVX2/AVX512 memcpy version */
    std::memcpy(cpu_index_buffer.data(), _idx_buffer.data(), _idx_buffer.size());
  }

  template <ct_string TStr>
  static constexpr usize
  get_name_index()
  {
    usize out = -1;
    for_constexpr<0, sizeof...(TAttributes), 1>(
        [&](auto ite)
        {
          using type = nth_type<ite, TAttributes...>;
          if constexpr (type::attribute_name == TStr.to_view())
          {
            out = ite;
          }
        });
    return out;
  }

  template <ct_string TName>
  static constexpr usize
  get_attribte_offset()
  {
    usize out  = 0;
    bool found = false;
    for_constexpr<0, sizeof...(TAttributes), 1>(
        [&](auto ite)
        {
          using type = nth_type<ite, TAttributes...>;
          if (type::attribute_name == TName.to_view())
          {
            found = true;
          }
          if (!found)
          {
            out += type::attribute_size;
          }
        });
    if (!found)
    {
      return -1;
    }
    else
    {
      return out;
    }
  }

  inline u0
  bind_buffers()
  {
    glBindVertexArray(vao_handle);
  }
};
} // namespace renderer::core