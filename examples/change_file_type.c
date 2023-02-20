#include <stdio.h>

typedef struct PlatformFile {
    FILE *handle;
} PlatformFile;

PlatformFile PlatformStdOut;


#define SAFE_PRINT_USE_OWN_FILE_OUTPUT
typedef PlatformFile SafePrintFileType;
#define SafePrintStdOut PlatformStdOut


#define SAFE_PRINT_IMPLEMENTATION
#include "../safe_print.h"
#define print safe_print


static void safe_print_output_character(SafePrintContext *context, char c) {
    if (fputc(c, context->file.handle) == EOF) {
        context->error = sp_true;
    } else {
        context->written += 1;
    }
}

static void safe_print_output_string(SafePrintContext *context, char const *str, size_t length) {
    size_t written = fwrite(str, 1, length, context->file.handle);
    context->written += written;
    if (written != length) {
        context->error = sp_true;
    }
}


int main(int argc, char **argv) {
    PlatformStdOut.handle = stdout;

    print("Number test: {}\n", 42);
}

