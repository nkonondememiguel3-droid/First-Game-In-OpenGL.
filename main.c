#include "glad/gl.h"
#include "load.h"
#include "utils.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  SDL_Window *window;

  bool is_running;
} _app_;

// our triangle vertex points.
float points[] = {
  0.0f,  0.5f,  0.0f, // xyz of the first point.
  0.5f,  -0.5f, 0.0f, // xyz of the second point.
  -0.5f, -0.5f, 0.0f, // xyz of the third point.
};
//float points[] = {
//  0.0f, 3.0f, 0.0f, 3.0f, -1.0f, 0.0f, -3.0f, -1.0f, 0.0f,
//};

int main( int argc, char *argv[] )
{

  int window_width = 800, window_height = 640;
  bool full_screen = false;
  float title_countdown_s = 0.1f;

  const char *vertex_shader_program = load_shader( "../shaders/vertex_shader_program.vert" );
  //const char *vertex_shader_program = load_shader( "../shaders/raymarcher.vert" );
  if ( vertex_shader_program == NULL )
  {
    error( "Failed to load the %s vertex shader mini-program.", "../shaders/vertex_shader_program.vert" );
    return EXIT_FAILURE;
  }

  const char *fragment_shader_grogram = load_shader( "../shaders/fragment_shader_program.frag" );
  //const char *fragment_shader_grogram = load_shader( "../shaders/raymarcher.frag" );
  if ( fragment_shader_grogram == NULL )
  {
    error( "Failed to load the %s vertex shader mini-program.", "../shaders/fragment_shader_program.frag" );
    return EXIT_FAILURE;
  }

  if ( !SDL_Init( SDL_INIT_VIDEO ) )
  {
    error( "Failed to initialize SDL3. ERROR: %s\n", SDL_GetError() );
    return EXIT_FAILURE;
  }

  _app_ application = {
    .is_running = true,
    .window = NULL,
  };

  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, true );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_CORE, SDL_GL_CONTEXT_PROFILE_CORE );
  /* SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 16 ); */

  // create a window.
  application.window = SDL_CreateWindow( "hello, world", window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
  if ( application.window == NULL )
  {
    error( "Failed to create a SDL3 window. ERROR: %s\n", SDL_GetError() );
    SDL_Quit();
    return EXIT_FAILURE;
  }
  SDL_GLContext gl_current_context = SDL_GL_CreateContext( application.window );
  if ( gl_current_context == NULL )
  {
    error( "Failed to create the current context. ERROR: %s\n", SDL_GetError() );
    SDL_DestroyWindow( application.window );
    SDL_Quit();
    return EXIT_FAILURE;
  }
  if ( !SDL_GL_MakeCurrent( application.window, gl_current_context ) )
  {
    error( "Failed to get the current context. ERROR: %s\n", SDL_GetError() );
    SDL_GL_DestroyContext( gl_current_context );
    SDL_DestroyWindow( application.window );
    SDL_Quit();
    return EXIT_FAILURE;
  }

  if ( !gladLoadGL( (GLADloadfunc)SDL_GL_GetProcAddress ) )
  {
    error( "Failed to initialize GLAD." );
    SDL_GL_DestroyContext( gl_current_context );
    SDL_DestroyWindow( application.window );
    SDL_Quit();
    return EXIT_FAILURE;
  }

  printf( "Renderer: %s\n", (char *)glGetString( GL_RENDERER ) );
  printf( "OpenGL version supported: %s\n", (char *)glGetString( GL_VERSION ) );

  unsigned int vbo_buffer = 0;
  glGenBuffers( 1, &vbo_buffer );
  glBindBuffer( GL_ARRAY_BUFFER, vbo_buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof( points ) / sizeof( points[0] ) * sizeof( float ), points, GL_STATIC_DRAW );

  unsigned int vao_buffer = 0;
  glGenVertexArrays( 1, &vao_buffer );
  glBindVertexArray( vao_buffer );
  glEnableVertexAttribArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, vbo_buffer );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

  GLuint vsp = create_shader( GL_VERTEX_SHADER, &vertex_shader_program );
  int params = -1;
  glGetShaderiv( vsp, GL_COMPILE_STATUS, &params ); // check for compilation errors.
  if ( GL_TRUE != params )
  {
    error_shader( vsp );
    SDL_GL_DestroyContext( gl_current_context );
    SDL_DestroyWindow( application.window );
    return EXIT_FAILURE;
  }

  GLuint fsp = create_shader( GL_FRAGMENT_SHADER, &fragment_shader_grogram );
  glGetShaderiv( fsp, GL_COMPILE_STATUS, &params ); // check for compilation errors.
  if ( GL_TRUE != params )
  {
    printf( "this is the error\n" );
    error_shader( fsp );
    SDL_GL_DestroyContext( gl_current_context );
    SDL_DestroyWindow( application.window );
    return EXIT_FAILURE;
  }

  GLuint shader_program = glCreateProgram();
  glAttachShader( shader_program, fsp );
  glAttachShader( shader_program, vsp );
  glLinkProgram( shader_program );

  glGetProgramiv( shader_program, GL_LINK_STATUS, &params );
  if ( GL_TRUE != params )
  {
    error_shader( shader_program );
    SDL_GL_DestroyContext( gl_current_context );
    SDL_DestroyWindow( application.window );
    return EXIT_FAILURE;
  }
  glDeleteShader( vsp );
  glDeleteShader( fsp );

  // update the uniform value in the vertex shader
  //int res_location = glGetUniformLocation( shader_program, "iResolution" );
  //int time_location = glGetUniformLocation( shader_program, "iTime" );
  int time_location = glGetUniformLocation( shader_program, "time" );
  //if ( time_location == -1 || res_location == -1 )
  if ( time_location == -1 )
  {
    //printf( "Attention: 'iTime' ou 'iResolution' introuvable dans le fragment shader.\n" );
    error( "Can't find 'time' in the vertex shader mini-program." );
    //error( "Attention: 'iTime' ou 'iResolution' introuvable dans le fragment shader.\n" );
    SDL_GL_DestroyContext( gl_current_context );
    SDL_DestroyWindow( application.window );
    glDeleteShader( vsp );
    glDeleteShader( fsp );
    return EXIT_FAILURE;
  }

  float prev_s = SDL_GetTicks();
  while ( application.is_running )
  {
    float curr_s = SDL_GetTicks();
    float elapsed_s = ( curr_s - prev_s ) / 1000.0f;
    prev_s = curr_s; // updates the previous time to be the current time.

    // handle events.
    SDL_Event event;
    while ( SDL_PollEvent( &event ) )
    {
      switch ( event.type )
      {
      case SDL_EVENT_QUIT:
        application.is_running = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        if ( event.key.key == SDLK_ESCAPE ) application.is_running = false;
        else if ( event.key.key == SDLK_F ) { full_screen = !full_screen; }
        SDL_SetWindowFullscreen( application.window, full_screen );
        break;
      default:
        break;
      }
    }

    // handle updates.
    SDL_GetWindowSize( application.window, &window_width, &window_height );
    int render_scale = 2;
    glViewport( 0, 0, window_width, window_height );

    title_countdown_s -= elapsed_s;
    if ( title_countdown_s <= 0.0 && elapsed_s > 0.0f )
    {
      float fps = 1.0 / elapsed_s; // miliseconds per frame.

      char tmp[256];
      sprintf( tmp, "fps@%.2f", fps );
      SDL_SetWindowTitle( application.window, tmp );

      title_countdown_s = 0.1f;
    }

    // handle drawing.
    glClearColor( 0.6f, 0.6f, 0.8f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( shader_program );
    //glProgramUniform3f( shader_program, res_location, (float)window_width, (float)window_height, 0.0f );

    float time = SDL_GetTicks() * 0.001f;
    glProgramUniform1f( shader_program, time_location, (float)time );

    glBindVertexArray( vao_buffer );

    glDrawArrays( GL_TRIANGLES, 0, 3 );

    SDL_GL_SwapWindow( application.window );
  }

  glDeleteBuffers( 1, &vbo_buffer );
  glDeleteVertexArrays( 1, &vao_buffer );
  glDeleteProgram( shader_program );

  SDL_GL_DestroyContext( gl_current_context );
  SDL_DestroyWindow( application.window );
  SDL_Quit();
  return EXIT_SUCCESS;
}
