#pragma once

#include "base_engine/renderer/shader.h"
#include <common.h>

#include <glad/glad.h>
#include <GL/gl.h>

#include <stack>
#include <cassert>
#include "base_texture.h"

template <typename TType>
struct wstack : std::stack<TType>
{
  /* this is the function you actually want to call trust me :) */
  TType
  wpop()
  {
    TType out = this->top();
    this->pop();
    return out;
  }
};

/*@FIXME: LEGACY class, should not be used anymore */
struct gl_fbo_texture
{
  gl_fbo_texture()  = default;
  ~gl_fbo_texture() = default;

  GLuint texture_handle, fbo_handle;
  u32 screen_w, screen_h;

  u0
  initialize(u32 _size_w, u32 _size_h)
  {
    /* fbo buffer allocation */
    glGenFramebuffers(1, &fbo_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);

    /* texture buffer allocation */
    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);

    /* set up texture */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _size_w, _size_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* bind texture to fbo */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_handle, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      std::cout << "[[error]] binding framebufferr! propably size (0,0)\n";
    }

    bind_screen_buffer();
  }

  u0
  destroy() const
  {
    glDeleteFramebuffers(1, &fbo_handle);
    glDeleteTextures(1, &texture_handle);
  }

  /* bind this if you want to update the texture */
  u0
  bind_frame_buffer()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
  }

  /* bind this if you want to use the exture to render something else */
  u0
  bind_texture()
  {
    glBindTexture(GL_TEXTURE_2D, texture_handle);
  }

  u0
  resize(u32 _x, u32 _y)
  {
    if (screen_w == _x && screen_h == _y)
    {
      return;
    }

    bind_screen_buffer();

    screen_w = _x;
    screen_h = _y;

    assert(texture_handle);
    assert(fbo_handle);

    glDeleteTextures(1, &texture_handle);
    glDeleteFramebuffers(1, &fbo_handle);

    /* fbo buffer allocation */
    glGenFramebuffers(1, &fbo_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);

    /* texture buffer allocation */
    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);

    /* set up texture */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _x, _y, 0, GL_RGBA, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* bind texture to fbo */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_handle, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      std::cout << "[[error]] binding framebufferr on resize! propably size (0,0)\n";
      assert(false);
    }

    bind_screen_buffer();
  }

  static u0
  bind_screen_buffer()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
};

struct pp_fs_texture
{
  /* bruh thanks openGL for not letting me use a string_view */
  const char* const vert_shader{
      R"(#version 430 core
          layout (location = 0) in vec2 aPos;
          layout (location = 1) in vec2 aTexCoords;
          out vec2 TexCoords;
    
          void main()
          {
            TexCoords = vec2(aTexCoords.x, aTexCoords.y);
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1); 
          }
      )"};

  pp_fs_texture() = default;

  u0
  initialize(u32 _screen_w, u32 _screen_h)
  {
    screen_w = _screen_w;
    screen_h = _screen_h;

    draw_shader.load_from_path("../pp_shader.vs", "../pp_shader.fs");

    pre_pass.initialize(_screen_w, _screen_h);

    static constexpr std::array verts{-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                                      -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  }

  pp_fs_texture(u32 _screen_w, u32 _screen_h) : screen_w(_screen_w), screen_h(_screen_h)
  {
    pre_pass.initialize(_screen_w, _screen_h);

    static constexpr std::array verts{-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                                      -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  }

  [[nodiscard("returns handle")]] usize
  add_post_pass(const char* const _frag_shader)
  {
    auto _name = new_handle();

    assert(!post_passes.contains(_name));

    GLuint vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex, 1, &vert_shader, NULL);
    glCompileShader(vertex);

    if (true)
    {
      GLint maxLength = 0;
      glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::string errorLog;
      errorLog.resize(maxLength);

      glGetShaderInfoLog(vertex, maxLength, &maxLength, &errorLog[0]);
      std::cout << "[ vertex shader comp ] : " << errorLog << "\n";
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment, 1, &_frag_shader, NULL);
    glCompileShader(fragment);
    {
      GLint maxLength = 0;
      glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::string errorLog;
      errorLog.resize(maxLength);

      glGetShaderInfoLog(fragment, maxLength, &maxLength, &errorLog[0]);
      std::cout << "[ fragment shader comp ] : " << errorLog << "\n";
    }

    auto shader_handle = glCreateProgram();

    glAttachShader(shader_handle, vertex);
    glAttachShader(shader_handle, fragment);
    glLinkProgram(shader_handle);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint status{0};
    glGetProgramiv(shader_handle, GL_VALIDATE_STATUS, &status);

    assert(status == GL_TRUE);

    /* fragment shader needs a screen_size and texture uniform */
    // assert(glGetUniformLocation(shader_handle, "screen_size") != -1);
    assert(glGetUniformLocation(shader_handle, "texture_diffuse") != -1);

    post_passes[_name] = std::pair{gl_fbo_texture{}, shader_handle};
    post_passes[_name].first.initialize(screen_w, screen_h);

    return _name;
  }

  u0
  remove_pass(usize _pass_name)
  {
    assert(post_passes.contains(_pass_name));
    post_passes.erase(_pass_name);
    delete_handle(_pass_name);
  }

  u0
  update_size(u32 _screen_w, u32 _screen_h)
  {
    if (_screen_w == screen_w && _screen_h == screen_h)
    {
      return;
    }

    screen_w = _screen_w;
    screen_h = _screen_h;

    pre_pass.resize(_screen_w, _screen_h);

    for (auto& pass : post_passes)
    {
      pass.second.first.resize(_screen_w, _screen_h);
    }
  }

  /* in here pass the render function that draws what you want to be modified by the shader */
  u0
  bake(usize _pass_name, auto _render_function)
  {
    assert(post_passes.contains(_pass_name));

    /* raw texture that we want to modify later*/
    pre_pass.bind_frame_buffer();

    /* render to pre pass fbo */
    _render_function();

    /*
    FIXME: do we need to do this every frame can we do it once on change?
    */

    /* use our shader */
    const GLuint shader_handle = post_passes.at(_pass_name).second;

    // glActiveTexture(GL_TEXTURE1); // Activate the texture unit (0 is the default)

    // glUniform2f(glGetUniformLocation(shader_handle, "screen_size"), (f32)screen_w, (f32)screen_h);
    glUseProgram(shader_handle);

    /* now bind the texture with the raw framebuffer */
    /* to draw it to the new fbo with our custom shader */
    post_passes.at(_pass_name).first.bind_frame_buffer();

    glBindVertexArray(vao);
    pre_pass.bind_texture();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // unbind all custom framebuffers
    gl_fbo_texture::bind_screen_buffer();
    glBindVertexArray(0);
  }

  // the the fullscreen texture to the screen consisting of all renderer passes
  u0
  draw(usize _pass_name)
  {
    assert(post_passes.contains(_pass_name));

    gl_fbo_texture::bind_screen_buffer();
    post_passes.at(_pass_name).first.bind_texture();
    draw_shader.use();

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
  }

  /* use this when you actually want to draw the texture on screen */
  u32
  get_texture_handle(usize _name)
  {
    assert(post_passes.contains(_name));
    return (u32)post_passes.at(_name).first.texture_handle;
  }

  ~pp_fs_texture()
  {
    for (auto& pass : post_passes)
    {
      glDeleteProgram(pass.second.second);
      pass.second.first.destroy();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
  }

private:
  usize last_buffer_handle{0};
  wstack<usize> returned_handles{};

  usize
  new_handle()
  {
    return returned_handles.empty() ? last_buffer_handle++ : returned_handles.wpop();
  }

  u0
  delete_handle(usize _handle)
  {
    if (_handle == last_buffer_handle - 1)
    {
      last_buffer_handle--;
    }
    else
    {
      returned_handles.push(_handle);
    }
  }

public:
  basic_shader_t draw_shader{"post process draw shader"};

  std::unordered_map<usize, std::pair<gl_fbo_texture, GLuint>> post_passes;
  gl_fbo_texture pre_pass;
  GLuint vbo, vao;

  /*
  FIXME: do we need these all instanced everywhere?
         it would be alot nicer if we had them once
         with the main backend handling their changing
         and all of these sub structures just having some
         sort of pointer/reference to those to avoid
         having situations where we dont update them
  */
  u32 screen_w, screen_h;
};