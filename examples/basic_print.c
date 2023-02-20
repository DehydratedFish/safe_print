#define SAFE_PRINT_IMPLEMENTATION
// NOTE: Define this for printing a useful message on error
// #define SAFE_PRINT_DEBUG

#include "../safe_print.h"


#define print safe_print


int main(int argc, char **argv) {
    print("Basic printing:\n");
    print("Hello world!\n\n");

    print("Application arg count: {}\n", argc);

    print("Application name: {}\n\n", argv[0]);

    print("Printing braces:\n");
    print("Left brace {{ | Right brace }}\n\n");

    print("Printing positional arguments:\n");
    print("1: {2}, 2: {3}, 3: {1}\n\n", "first arg", "second arg", "third arg");

    print("Upper and lowercase strings:\n");
    print("Uppercase: {upper}\n", "Going Postal!");
    print("Lowercase: {lower}\n\n", "Going Postal!");

    print("Keep sign: {sign}\n", 42);
    print("Keep sign: {sign}\n", -42);
    print("Keep sign: {sign}\n\n", 42.42);

    print("Printing 42 in different bases:\n");
    print("{bin}\n", 42);
    print("{oct}\n", 42);
    print("{dec}\n", 42);
    print("{hex}\n\n", 42);

    print("Hexadecimal in uppercase:\n");
    print("{hex:upper}\n\n", 0xdeadbeefULL);

    print("Scientific and hex floats:\n");
    print("{sci}\n", 42.42);
    print("{sci:upper}\n", 42.42);
    print("{hex}\n", 42.42);
    print("{hex:upper}\n\n", 42.42);

    print("Minimum width with fill:\n");
    print("1: {min(8)}|{min(8):fill(-)}|{min(8):fill(*):left}\n", 500, 200, 40);
    print("2: {min(8):fill(*)}|{min(8)}|{min(8):fill(-):left}\n\n", 500, 184412200, 40);

    print("{min(30)}\n", "left");
    print("{min(30):right}\n", "right");
    print("{min(30):fill(@)}\n", "here");
}

