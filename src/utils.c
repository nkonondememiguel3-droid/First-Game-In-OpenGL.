//
// Created by nkono on 7/17/2026.
//

#include "include/utils.h"

#include <SDL3/SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "glad/gl.h"

void error( const char *fmt, ... )
{
  va_list args;
  char *msg = NULL;
  char *full = NULL;

  va_start( args, fmt );
  SDL_vasprintf( &msg, fmt, args );
  va_end( args );

  if ( msg )
  {
    SDL_asprintf( &full, "ERROR: %s", msg );
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "error", full, NULL );
    SDL_free( msg );
    SDL_free( full );

    exit( EXIT_FAILURE );
  }
}

void error_shader( unsigned int shader )
{
  int max_length = 2048, actual_length = 0;
  char xlog[2048] = { 0 }; // Initialize to clear memory

  // Check if it's a shader or a program
  if ( glIsShader( shader ) )
  {
    glGetShaderInfoLog( shader, max_length, &actual_length, xlog );
    fprintf( stderr, "ERROR: Shader GL index %u failed compilation.\n%s\n", shader, xlog );
  }
  else if ( glIsProgram( shader ) )
  {
    glGetProgramInfoLog( shader, max_length, &actual_length, xlog );
    fprintf( stderr, "ERROR: Program GL index %u failed linking.\n%s\n", shader, xlog );
  }

  error( "OpenGL Error (Index %u):\n%s\n", shader, xlog );
}

GLuint create_shader( GLenum shader_type, const GLchar *const *source )
{
  unsigned int sp = glCreateShader( shader_type );
  glShaderSource( sp, 1, source, NULL );
  glCompileShader( sp );

  return sp;
}
