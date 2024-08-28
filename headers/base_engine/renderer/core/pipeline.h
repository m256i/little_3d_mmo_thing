#pragma once

#include <common.h>
#include <vector>

/*
auto pipeline = model_drawbuffer > pipeline::combine{draw_shader > screen_tex, ssao_shader > ssao_tex} > combine_shader >
pipeline::screen_buffer;
*/

namespace renderer::core
{

struct vertex_buffer_pipeline_stage
{

  /*
  TODO: final pipeline stage that has this function not a random one like this
  */
  u0
  evaluate()
  {
    for (i32 i = pipeline_stage.size() - 1; i != -1; i--)
    {
      pipeline_stage[i]();
    }
  }
  std::vector<std::function<void()>> pipeline_stage{};
};

template <typename TShaderInputTuple>
struct shader_pipeline_stage
{
  static constexpr TShaderInputTuple shader_inputs{};
  std::vector<std::function<void()>> pipeline_stage{};
};

template <usize TColorAttachementCount>
struct framebuffer_pipeline_stage
{
  std::vector<std::function<void()>> pipeline_stage{};
};

/*
TODO: screen framebuffer pipeline stage

TODO: pipeline combination and also putting textures into shaders with an implied vertex buffer (fullscreen quad) maybe?
*/

} // namespace renderer::core