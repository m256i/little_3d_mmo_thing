#pragma once

#include "../mesh.h"
#include "../shader.h"
#include "headers/base_engine/renderer/shader.h"

struct point_light_t
{
  glm::vec3 position;
};

struct static_lighting_t
{
  bool
  bake_lights_to_texture(mesh_t& _mesh, std::vector<u8>& _out_tex_buffer)
  {
    for (const auto& texture : _mesh.textures)
    {
      u32 fbo;
      glGenFramebuffers(1, &fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);

      u32 out_texture_id{};
      glGenTextures(1, &out_texture_id);
      glBindTexture(GL_TEXTURE_2D, out_texture_id);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_texture_id, 0);

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      {
        LOG(INFO) << "failed to generate lighting texture buffer";
        return false;
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      basic_shader_t baking_shader{"baking_shader", "../basic_model.vs", "../light_baking_shader.fs"};

      _out_tex_buffer.resize(texture.width * texture.height * 4);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glReadPixels(0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, _out_tex_buffer.data());
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  }
};
