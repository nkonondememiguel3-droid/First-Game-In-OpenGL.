//
// Created by nkono on 7/17/2026.
//

#include "include/utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#include "glad/gl.h"

void error(const char *fmt, ...) {
    va_list args;
    char *msg = NULL;
    char *full = NULL;

    va_start(args, fmt);
    SDL_vasprintf(&msg, fmt, args);
    va_end(args);

    if (msg) {
        SDL_asprintf(&full, "ERROR: %s", msg);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "error", full, NULL);
        SDL_free(msg);
        SDL_free(full);

        exit(EXIT_FAILURE);
    }
}

void error_shader( unsigned int shader )
{
    int max_length = 2048, actual_length = 0;
    char xlog[2058];
    glGetShaderInfoLog( shader, max_length, &actual_length, xlog );
    fprintf( stderr, "ERROR: Could not be link shader program GL index %u.\n%s\n", shader, xlog );
    error( "ERROR: Shader index %u did not compile.\n%s\n", shader, xlog );
}

GLuint create_shader(GLenum shader_type, const GLchar *const* source) {
    unsigned int sp = glCreateShader(shader_type);
    glShaderSource( sp, 1, source, NULL );
    glCompileShader( sp );

    return sp;
}
