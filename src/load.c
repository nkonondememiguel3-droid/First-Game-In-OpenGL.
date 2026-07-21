//
// Created by nkono on 7/20/2026.
//

#include <stdio.h>
#include <stdlib.h>

const char *load_shader(const char *file_path) {
    if (file_path == NULL) return NULL;

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to read the content of %s shader file.\n", file_path);
        return NULL;
    }

    size_t file_size = 0;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    printf("file size: %llu\n", file_size);

    char *buffer = (char *) malloc( file_size * sizeof( char ) );
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocator %llu amount of memor.\n", file_size);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    printf("Read size: %llu\n", read_size);

    fclose(fp);

    return buffer;
}
