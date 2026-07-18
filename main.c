#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"
#include "utils.h"
#include "glad/gl.h"

typedef struct {
    SDL_Window *window;

    bool is_running;
} _app_;

// where our vertex pixels should be place on the screen.
const char * vertex_shader_program = ""
"#version 410 core\n"
"in vec3 vp;"
"void main() {"
"   gl_Position = vec4( vp, 1.0 );"
"}";

// the color used by our pixel-framed vertex.
const char *fragment_shader_grogram = ""
"#version 410 core\n"
"out vec4 frag_colour;"
"void main() {"
"   frag_colour = vec4( 0.5, 0.0, 0.5, 1.0 );"
"}";

int main(int argc, char *argv[]) {

    const int window_width = 800, window_height = 640;

    if ( !SDL_Init( SDL_INIT_VIDEO ) ) {
        error("Failed to initialize SDL3. ERROR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    _app_ application = {
        .is_running = true,
        .window = NULL,
    };

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, true);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_CORE, SDL_GL_CONTEXT_PROFILE_CORE);

    // create a window.
    application.window = SDL_CreateWindow("hello, world", window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (application.window == NULL) {
        error("Failed to create a SDL3 window. ERROR: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_GLContext gl_current_context = SDL_GL_CreateContext(application.window);
    if (gl_current_context == NULL) {
        error("Failed to create the current context. ERROR: %s\n", SDL_GetError());
        SDL_DestroyWindow(application.window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    if (!SDL_GL_MakeCurrent(application.window, gl_current_context)) {
        error("Failed to get the current context. ERROR: %s\n", SDL_GetError());
        SDL_GL_DestroyContext(gl_current_context);
        SDL_DestroyWindow(application.window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        error("Failed to initialize GLAD. ERROR: %s\n", glGetError());
    }

    printf("Renderer: %s\n", (char *)glGetString(GL_RENDERER));
    printf("OpenGL version supported: %s\n", (char *)glGetString(GL_VERSION));

    // our triangle points.
    float points[] = {
        0.0f, 0.5f, 0.0f, // xyz of the first point.
        0.5f, -0.5f, 0.0f, // xyz of the second point.
        -0.5f, -0.5f, 0.0f, // xyz of the third point.
    };

    GLuint vbo_buffer = 0;
    glGenBuffers(1, &vbo_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) / sizeof(points[0]) * sizeof(float), points, GL_STATIC_DRAW);

    unsigned int vao_buffer = 0;
    glGenVertexArrays(1, &vao_buffer);
    glBindVertexArray(vao_buffer);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    GLuint vsp = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsp, 1, &vertex_shader_program, NULL);
    glCompileShader(vsp);

    unsigned int fsp = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fsp, 1, &fragment_shader_grogram, NULL);
    glCompileShader(fsp);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fsp);
    glAttachShader(shader_program, vsp);
    glLinkProgram(shader_program);

    while (application.is_running) {
        // handle events.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    application.is_running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) application.is_running = false;
                    break;
                default:
                    break;
            }
        }

        // handle drawing.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(vao_buffer);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(application.window);

    }

    SDL_GL_DestroyContext(gl_current_context);
    SDL_DestroyWindow(application.window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
