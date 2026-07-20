//
// Created by nkono on 7/17/2026.
//

#ifndef FIRST_OPENGL_GAME_UTILS_H
#define FIRST_OPENGL_GAME_UTILS_H
#include "glad/gl.h"

void error(const char *fmt, ...);
void error_shader( unsigned int shader );
GLuint create_shader(GLenum shader_type, const GLchar *const* source);

#endif //FIRST_OPENGL_GAME_UTILS_H
