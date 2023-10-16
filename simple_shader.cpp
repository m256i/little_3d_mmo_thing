#include "simple_shader.h"

simple_shader_t::simple_shader_t(const char* vertex_path, const char* fragment_path)
{
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try
  {

    vShaderFile.open(vertex_path);
    fShaderFile.open(fragment_path);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexCode   = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  }
  catch (std::ifstream::failure e)
  {
    std::cout << "[logging]: couldnt open file" << std::endl;
  }
  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();

  unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vShaderCode, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "[logging]: vertex shader\n" << infoLog << std::endl;
  }

  unsigned fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "[logging]: fragment shader\n" << infoLog << std::endl;
  }

  this->index = glCreateProgram();
  glAttachShader(this->index, vertexShader);
  glAttachShader(this->index, fragmentShader);
  glLinkProgram(this->index);

  glGetProgramiv(this->index, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(this->index, 512, NULL, infoLog);
    std::cout << "[logging]: linking shader\n" << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void
simple_shader_t::use() const
{
  glUseProgram(this->index);
}

void
simple_shader_t::set_bool(const std::string& name, bool value) const
{
  glUniform1i(glGetUniformLocation(this->index, name.c_str()), (int)value);
}
void
simple_shader_t::set_int(const std::string& name, int value) const
{
  glUniform1i(glGetUniformLocation(this->index, name.c_str()), value);
}
void
simple_shader_t::set_float(const std::string& name, float value) const
{
  glUniform1f(glGetUniformLocation(this->index, name.c_str()), value);
}

void
simple_shader_t::set_mat4(const std::string& name, glm::mat4 value) const
{
  glUniformMatrix4fv(glGetUniformLocation(this->index, name.c_str()), 1, GL_FALSE, &value[0][0]);
}