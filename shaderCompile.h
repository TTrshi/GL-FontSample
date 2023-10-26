#pragma once


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

GLint Shader_load(const char* shader_source, const GLenum GL_XXXX_SHADER);
GLuint Shader_createProgramFromSource(const char* vertex_shader_source, const char* fragment_shader_source);
