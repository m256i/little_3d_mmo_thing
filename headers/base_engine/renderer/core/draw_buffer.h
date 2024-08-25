#pragma once

#include <common.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <cassert>

namespace renderer::core
{
/*
we support sending a fixed list of types to the GPU:
float
int
uint
vec2f
vec3f
vec4f
mat3f
mat4f
*/

using allowed_types = std::tuple<f32, i32, u32, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

template <typename TType>
struct glsl_type_name_map;

template <typename TType>
struct glsl_type_translation;

template <>
struct glsl_type_name_map<f32>
{
  constexpr static std::string_view value{"float"};
};

template <>
struct glsl_type_translation<f32>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(f32) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT;
};

template <>
struct glsl_type_name_map<i32>
{
  constexpr static std::string_view value{"int"};
};

template <>
struct glsl_type_translation<i32>
{
  constexpr static usize type_size  = sizeof(i32);
  constexpr static usize type_count = sizeof(i32) / sizeof(i32);
  constexpr static GLenum gl_type   = GL_INT;
};

template <>
struct glsl_type_name_map<u32>
{
  constexpr static std::string_view value{"uint"};
};

template <>
struct glsl_type_translation<u32>
{
  constexpr static usize type_size  = sizeof(u32);
  constexpr static usize type_count = sizeof(u32) / sizeof(u32);
  constexpr static GLenum gl_type   = GL_UNSIGNED_INT;
};

template <>
struct glsl_type_name_map<glm::vec2>
{
  constexpr static std::string_view value{"vec2"};
};

template <>
struct glsl_type_translation<glm::vec2>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::vec2) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT;
};

template <>
struct glsl_type_name_map<glm::vec3>
{
  constexpr static std::string_view value{"vec3"};
};

template <>
struct glsl_type_translation<glm::vec3>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::vec3) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT;
};

template <>
struct glsl_type_name_map<glm::vec4>
{
  constexpr static std::string_view value{"vec4"};
};

template <>
struct glsl_type_translation<glm::vec4>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::vec4) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT;
};

template <>
struct glsl_type_name_map<glm::mat3>
{
  constexpr static std::string_view value{"mat3"};
};

template <>
struct glsl_type_translation<glm::mat3>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::mat3) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT;
};

template <>
struct glsl_type_name_map<glm::mat4>
{
  constexpr static std::string_view value{"mat4"};
};

template <>
struct glsl_type_translation<glm::mat4>
{
  constexpr static usize type_size  = sizeof(f32);
  constexpr static usize type_count = sizeof(glm::mat4) / sizeof(f32);
  constexpr static GLenum gl_type   = GL_FLOAT;
};

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

template <typename T>
struct is_drawbuf_attrib : std::false_type
{
};

template <ct_string TName, typename... TTypes>
struct is_drawbuf_attrib<drawbuf_attrib<TName, TTypes...>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_drawbuf_attrib_v = is_drawbuf_attrib<T>::value;

/*
the class that does the thing!
*/
template <typename... TAttributes>
struct static_drawbuffer
{
  static_assert((is_drawbuf_attrib_v<TAttributes> && ...), "all parameters must be instantiations of drawbuf_attrib.");

  /*
  ordering sequential! always!
  */
  constexpr static std::tuple<TAttributes...> attribute_row;
  std::vector<std::tuple<TAttributes...>> vertex_data_cpu_buffer{};
  std::vector<i32> cpu_index_buffer{};

  bool initialized = false;

  u32 vao_handle{}, vbo_handle{}, ebo_handle{};

  bool
  initialize_gpu_buffer()
  {
    if (initialized)
    {
      return false;
    }

    assert(vertex_data_cpu_buffer.size() == cpu_index_buffer.size());

    glGenVertexArrays(1, &vao_handle);
    glGenBuffers(1, &vbo_handle);
    glGenBuffers(1, &ebo_handle);

    /*
    buffer the vert data
    */
    glBindVertexArray(vao_handle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_cpu_buffer.size() * sizeof(attribute_row), vertex_data_cpu_buffer.data(), GL_STATIC_DRAW);

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
    return true;
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
};
} // namespace renderer::core