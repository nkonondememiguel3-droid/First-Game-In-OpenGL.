//
// Created by nkono on 7/17/2026.
//

#include "include/utils.h"

#include <stdarg.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

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
