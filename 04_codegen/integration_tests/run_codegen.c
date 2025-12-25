#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>

static char *read_file(const char *path)
{
    FILE *file = fopen(path, "rb");
    char *buffer = NULL;
    long size = 0;
    size_t read_bytes = 0;

    if (!file) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    size = ftell(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    buffer = malloc((size_t)size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    read_bytes = fread(buffer, 1, (size_t)size, file);
    buffer[read_bytes] = '\0';

    fclose(file);
    return buffer;
}

int main(int argc, char **argv)
{
    const char *input_path = NULL;
    const char *output_path = NULL;
    char *source = NULL;
    Codegen codegen;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <input.c> <output.ll>\n", argv[0]);
        return 1;
    }

    input_path = argv[1];
    output_path = argv[2];

    source = read_file(input_path);
    if (!source) {
        fprintf(stderr, "failed to read %s\n", input_path);
        return 1;
    }

    codegen_init(&codegen, source);
    if (!codegen_emit(&codegen, output_path)) {
        fprintf(stderr, "codegen error: %s\n", codegen_error(&codegen));
        free(source);
        return 1;
    }

    free(source);
    return 0;
}
