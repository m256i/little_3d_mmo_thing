#pragma once

#include <common.h>
#include <vector>
#include "draw_buffer.h"
#include "frame_buffer.h"
#include "shader.h"
#include "base_texture.h"

/*
auto pipeline = model_drawbuffer > pipeline::combine{draw_shader > screen_tex, ssao_shader > ssao_tex} > combine_shader >
pipeline::screen_buffer;
*/

namespace renderer::core::detail
{
template <renderer::core::drawbuffer_type TDrawBufType, usize TTextureCount, typename... TAttributes>
struct buffer_texture_list_begin
{
  i32 current_texture_index{};
  std::vector<std::function<void()>> functions{};
};

template <renderer::core::drawbuffer_type TDrawBufType, usize TTextureCount, typename... TAttributes>
struct texture_texture_combinator
{
  i32 current_texture_index{};
  std::vector<std::function<void()>> functions{};
};

} // namespace renderer::core::detail

/*
TODO: chain multiple textures not supportededete rn!!! hahah!!
*/

/*
take a draw buffer and a texture and combine them to a buffer_texture_list_begin node
*/
template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes>
inline renderer::core::detail::buffer_texture_list_begin<TDrawBufType, 1, TAttributes...>
operator|(renderer::core::static_drawbuffer<TDrawBufType, TAttributes...> _drawbuffer, renderer::core::base_texture2d& _texture)
{
  LOG(DEBUG) << "leaving 1";
  std::vector<std::function<void()>> out{[&]()
                                         {
                                           LOG(INFO) << "binding draw buffer";
                                           _drawbuffer.bind_buffers();
                                           glActiveTexture(GL_TEXTURE0);
                                           LOG(INFO) << "binding texture" << 0;
                                           _texture.bind();
                                         }};
  return renderer::core::detail::buffer_texture_list_begin<TDrawBufType, 1, TAttributes...>{.current_texture_index{1}, .functions{out}};
}

/*
take a buffer_texture_list_begin node and another texture and combine them
*/
template <renderer::core::drawbuffer_type TDrawBufType, usize TTexCount, typename... TAttributes>
inline renderer::core::detail::texture_texture_combinator<TDrawBufType, TTexCount + 1, TAttributes...>
operator|(renderer::core::detail::buffer_texture_list_begin<TDrawBufType, TTexCount, TAttributes...> _begin,
          renderer::core::base_texture2d& _next)
{
  LOG(DEBUG) << "leaving 2";

  std::vector<std::function<void()>> out{_begin.functions};

  out.push_back(
      [&]()
      {
        glActiveTexture(GL_TEXTURE0 + TTexCount);
        LOG(INFO) << "binding texture" << TTexCount;
        _next.bind();
      });

  return renderer::core::detail::texture_texture_combinator<TDrawBufType, TTexCount + 1, TAttributes...>{
      .current_texture_index{_begin.current_texture_index + 1}, .functions{out}};
}

/*
take a texture_texture_combinator node and another texture and combine them
*/
template <renderer::core::drawbuffer_type TDrawBufType, usize TTexCount, typename... TAttributes>
inline renderer::core::detail::texture_texture_combinator<TDrawBufType, TTexCount + 1, TAttributes...>
operator|(renderer::core::detail::texture_texture_combinator<TDrawBufType, TTexCount, TAttributes...> _left_texture,
          renderer::core::base_texture2d& _right_texture)
{
  LOG(DEBUG) << "leaving 3";

  std::vector<std::function<void()>> out{_left_texture.functions};

  out.push_back(
      [&]()
      {
        glActiveTexture(GL_TEXTURE0 + TTexCount);
        LOG(INFO) << "binding texture" << TTexCount;
        _right_texture.bind();
      });

  return renderer::core::detail::texture_texture_combinator<TDrawBufType, TTexCount + 1, TAttributes...>{
      .current_texture_index{_left_texture.current_texture_index + 1}, .functions{out}};
}

/*
TODO: need 2 versions
one with texture_texture_combinator for more than 1 texture
and one with buffer_texture_list_begin for only one texture
*/
namespace renderer::core
{
template <renderer::core::drawbuffer_type TDrawBufType, usize TTexCount, typename... TAttributes>
struct combine
{
  /*
  if there is multiple textures bound in the thingy we expect to get a texture_texture_combinator from the operators
  */
  combine(const renderer::core::detail::texture_texture_combinator<TDrawBufType, TTexCount, TAttributes...>& _aggregate)
  {
    functions = _aggregate.functions;
  }
  /*
  if there is only one texture we want buffer_texture_list_begin
  */
  combine(const renderer::core::detail::buffer_texture_list_begin<TDrawBufType, TTexCount, TAttributes...>& _aggregate)
  {
    functions = _aggregate.functions;
  }

  std::vector<std::function<void()>> functions{};
};
} // namespace renderer::core

namespace renderer::core
{

struct framebuffer_pipeline_stage
{
  std::vector<std::function<void()>> functions{};

  u0
  evaluate()
  {
    for (const auto& func : functions)
    {
      func();
    }
  }
};

template <usize TOutColorAttachements>
struct drawbuf_shader_stage
{
  std::vector<std::function<void()>> functions{};
};

/*
TODO: screen framebuffer pipeline stage
*/

} // namespace renderer::core

/*
TODO: make sure to fix up initalize functions and call them here!
*/

/*
TODO: match shader input textures against this here using name (add name template to base_texture2d)
*/

/*
attach a shader to a draw/vertex buffer module
*/
template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes, ct_string TShaderName, ct_string TShaderPath,
          typename... TShaderInOutNodes>
inline auto
operator>(renderer::core::static_drawbuffer<TDrawBufType, TAttributes...>& _drawbuffer,
          renderer::core::shader<TShaderName, TShaderPath, TShaderInOutNodes...>& _shader)
    -> renderer::core::drawbuf_shader_stage<std::tuple_size_v<decltype(_shader.shader_outputs)>>
{
  static_assert(_drawbuffer.template shader_matches_drawbuf<decltype(_shader.shader_inputs)>(),
                "framebufer attributes and shader inputs are different");

  using namespace renderer::core;
  renderer::core::drawbuf_shader_stage<std::tuple_size_v<decltype(_shader.shader_outputs)>> out;
  out.functions.push_back(
      [&]()
      {
        LOG(INFO) << "binding drawbuffer and shader without any textures!";
        _drawbuffer.bind_buffers();
        /*
        when its called where that means there is no textures so just bind nothing
        */
        _shader.bind();
      });
  return out;
}

namespace detail
{
template <std::size_t TTextureCount>
consteval static auto
gen_texid_array()
{
  std::array<u32, TTextureCount> out_arr{};
  u32 i = 0;
  for (auto& ite : out_arr)
  {
    ite = i++;
  }
  return out_arr;
};
} // namespace detail

template <renderer::core::drawbuffer_type TDrawBufType, usize TTexCount, typename... TAttributes, ct_string TShaderName,
          ct_string TShaderPath, typename... TShaderInOutNodes>
inline auto
operator>(const renderer::core::combine<TDrawBufType, TTexCount, TAttributes...>& _drawbuffer,
          renderer::core::shader<TShaderName, TShaderPath, TShaderInOutNodes...>& _shader)
    -> renderer::core::drawbuf_shader_stage<std::tuple_size_v<decltype(_shader.shader_outputs)>>
{
  static_assert(
      renderer::core::static_drawbuffer<TDrawBufType, TAttributes...>::template shader_matches_drawbuf<decltype(_shader.shader_inputs)>(),
      "framebufer attributes and shader inputs are different");

  static_assert(TTexCount == std::tuple_size_v<decltype(_shader.texture_inputs)>,
                "amount of shader texture inputs and textures in 'combine{}' do not match");

  using namespace renderer::core;
  if constexpr (TTexCount == std::tuple_size_v<decltype(_shader.texture_inputs)>)
  {
    {
      renderer::core::drawbuf_shader_stage<std::tuple_size_v<decltype(_shader.shader_outputs)>> out;

      std::cout << "Texture count: " << TTexCount << "\n";

      for (const auto& func : _drawbuffer.functions)
      {
        out.functions.push_back(func);
      }

      out.functions.push_back(
          [&]()
          {
            /*
            FIXME: combine{} should handle binding drawbuffers and textures;
            */
            LOG(INFO) << "binding texture indices: ";
            for (const auto ite : ::detail::gen_texid_array<TTexCount>())
            {
              std::cout << "  " << ite;
            }
            std::cout << "\n";

            //_drawbuffer.bind_buffers();
            _shader.bind(::detail::gen_texid_array<TTexCount>());
            LOG(INFO) << "binding shader";
          });

      LOG(DEBUG) << "leaving";
      return out;
    }
  }
  else
  {
    /* if constexpr quirk */
    std::unreachable();
  }
}

template <usize TShaderOutColorAttachments, renderer::core::frame_buffer_options TOptions,
          renderer::core::base_texture2d::texture_format... TColorAttachments>
inline renderer::core::framebuffer_pipeline_stage
operator>(renderer::core::drawbuf_shader_stage<TShaderOutColorAttachments> _shader,
          renderer::core::frame_buffer<TOptions, TColorAttachments...>& _framebuffer)
{
  static_assert(TShaderOutColorAttachments == sizeof...(TColorAttachments), "shader and framebuffer have mismatch in color in/outputs");
  using namespace renderer::core;
  framebuffer_pipeline_stage out;
  for (const auto& stage : _shader.functions)
  {
    out.functions.push_back(stage);
  }
  out.functions.push_back(
      [&]()
      {
        LOG(INFO) << "binding framebuffer!";
        _framebuffer.bind();
      });
  return out;
}

/*
attach a framebuffer to a shaderoutput
*/