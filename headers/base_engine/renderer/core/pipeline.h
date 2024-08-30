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
template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes>
struct buffer_texture_list_begin
{
  i32 current_texture_index{};
};

template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes>
struct texture_texture_combinator
{
  i32 current_texture_index{};
};

} // namespace renderer::core::detail

/*
TODO: chain multiple textures not supportededete rn!!! hahah!!
*/

template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes>
inline renderer::core::detail::buffer_texture_list_begin<TDrawBufType, TAttributes...>
operator|(renderer::core::static_drawbuffer<TDrawBufType, TAttributes...> _drawbuffer, const renderer::core::base_texture2d& _texture)
{
  return renderer::core::detail::buffer_texture_list_begin<TDrawBufType, TAttributes...>{1};
}

template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes>
inline renderer::core::detail::texture_texture_combinator<TDrawBufType, TAttributes...>
operator|(renderer::core::detail::buffer_texture_list_begin<TDrawBufType, TAttributes...> _begin,
          const renderer::core::base_texture2d& _next)
{
  return renderer::core::detail::texture_texture_combinator<TDrawBufType, TAttributes...>{_begin.current_texture_index + 1};
}

namespace renderer::core
{
template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes>
struct combine
{
  renderer::core::detail::texture_texture_combinator<TDrawBufType, TAttributes...> aggregate{};
};
} // namespace renderer::core

namespace renderer::core
{

struct framebuffer_pipeline_stage
{
  std::vector<std::function<void()>> functions{};
};

template <usize TOutColorAttachements>
struct drawbuf_shader_stage
{
  std::vector<std::function<void()>> functions{};
};

/*
TODO: screen framebuffer pipeline stage

TODO: pipeline combination and also putting textures into shaders with an implied vertex buffer (fullscreen quad) maybe?
*/

} // namespace renderer::core

/*
TODO: make sure to fix up initalize functions and call them here!
*/

/*
TODO: match shader input textures against this here
*/

/*
attach a shader to a draw/vertex buffer module
*/
template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes, ct_string TShaderName, ct_string TShaderPath,
          typename... TShaderInOutNodes>
inline auto
operator>(renderer::core::static_drawbuffer<TDrawBufType, TAttributes...> _drawbuffer,
          renderer::core::shader<TShaderName, TShaderPath, TShaderInOutNodes...> _shader)
    -> renderer::core::drawbuf_shader_stage<std::tuple_size_v<decltype(_shader.shader_outputs)>>
{
  static_assert(_drawbuffer.template shader_matches_drawbuf<decltype(_shader.shader_inputs)>(),
                "framebufer attributes and shader inputs are different");

  using namespace renderer::core;
  renderer::core::drawbuf_shader_stage<std::tuple_size_v<decltype(_shader.shader_outputs)>> out;
  out.functions.push_back(
      [&]()
      {
        _drawbuffer.bind_buffers();
        /*
        when its called where that means there is no textures so just bind nothing
        */
        _shader.bind();
      });
  return out;
}

template <renderer::core::drawbuffer_type TDrawBufType, typename... TAttributes, ct_string TShaderName, ct_string TShaderPath,
          typename... TShaderInOutNodes>
inline auto
operator>(renderer::core::combine<TDrawBufType, TAttributes...> _drawbuffer,
          renderer::core::shader<TShaderName, TShaderPath, TShaderInOutNodes...> _shader)
    -> renderer::core::drawbuf_shader_stage<std::tuple_size_v<decltype(_shader.shader_outputs)>>
{
  /*
  TODO: check here used to be done by drawbuffer;
  */
  // static_assert(_drawbuffer.template shader_matches_drawbuf<decltype(_shader.shader_inputs)>(),
  //               "framebufer attributes and shader inputs are different");

  using namespace renderer::core;
  renderer::core::drawbuf_shader_stage<std::tuple_size_v<decltype(_shader.shader_outputs)>> out;
  out.functions.push_back(
      [&]()
      {
        /*
        FIXME: combine{} should handle binding drawbuffers and textures;
        */

        //_drawbuffer.bind_buffers();
        /*
        when its called where that means there is no textures so just bind nothing
        */
        _shader.bind({/*TODO: generate array with texture idxs from ctime counter in here thats passed as NTTP*/});
      });
  return out;
}

template <usize TShaderOutColorAttachments, renderer::core::frame_buffer_options TOptions,
          renderer::core::base_texture2d::texture_format... TColorAttachments>
inline renderer::core::framebuffer_pipeline_stage
operator>(renderer::core::drawbuf_shader_stage<TShaderOutColorAttachments> _shader,
          renderer::core::frame_buffer<TOptions, TColorAttachments...> _framebuffer)
{
  static_assert(TShaderOutColorAttachments == sizeof...(TColorAttachments), "shader and framebuffer have mismatch in color in/outputs");
  using namespace renderer::core;
  framebuffer_pipeline_stage out;
  for (const auto& stage : _shader.functions)
  {
    out.functions.push_back(stage);
  }
  out.functions.push_back([&]() { _framebuffer.bind(); });
}

/*
attach a framebuffer to a shaderoutput
*/