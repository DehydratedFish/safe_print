#define SAFE_PRINT_IMPLEMENTATION
// NOTE: Define this for printing a useful message on error
// #define SAFE_PRINT_DEBUG

#include "../safe_print.h"


#define log safe_print_file


int main(int argc, char **argv) {
    FILE *log_file = fopen("log.txt", "w");

    log(log_file, "ERROR: {} is not the answer to everything.\n\n", 42);
    log(log_file, "NOTE: {upper}\n", "please yell!");

    fclose(log_file);
}

