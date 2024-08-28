#pragma once

#include <common.h>
#include <stack>

/*


auto pipeline = model_drawbuffer > pipeline::combine{draw_shader > screen_tex, ssao_shader > ssao_tex} > combine_shader > screen_tex;


pipeline.evaluate();


*/

namespace renderer::core
{

enum class pipeline_stage_type
{
  none,
  vertices,
  fragments,
  texture
};

struct vertex_buffer_pipeline_stage
{
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

} // namespace renderer::core