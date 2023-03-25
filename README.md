# safe_print

## Introduction
This header is trying to provide a better solution to C's printf-like
functions.

The format parser is probably a bit overkill, but I had fun building it that
way. So far I have no intention to make any performance analysis or changes.
This is more a proof of concept and not a meaningful or complete
implementation.

Unfortunately the library is only available under C11. Support for
variadic macros and `_Generic` is needed for everything to work.
Also to count the number of variadic arguments an extension is needed for
GCC. You have to compile with `-std=gnu11` instead of `-std=c11` for it to work.
On MSVC the behavior is the default.

As you probably know those are not type-safe and you need to manually
specify how the next argument has to be interpreted. Modern compilers
have some checks to make sure that all format specifiers match the provided
arguments, but that is still tedious and not all platforms use the same
specifiers for their types. (e.g. `long` can be 32 or 64 bits) Or don't even
support all specifiers. (see `size_t` and `%zu`)
The function uses a python like syntax with `{}` as a placeholder for
the replacement. But the syntax is different.
The library currently only supports 16 arguments to the print routine.

## Usage

This single header file is inspired by Sean Barret's headers.
Define `SAFE_PRINT_IMPLEMENTATION` in a .c file where you want the
implementation to be and then `#include` this header.
All other files now only need to include the header without anything else.

Call the macro `safe_print()` with a format string and the required
arguments. For example:

    safe_print("Hello {}!\n", "World");

This will print the well known string `"Hello World"` to `stdout`.
By default the library will use the standart CRT file API.

    FILE *file = fopen("log.txt", "w");
    safe_print_file(file, "The answer is: {}\n", 42);

This will print `"The answer is: 42"` to the file log.txt.

You can find some more use cases in the examples folder.

Format specifiers:
- `1 to 16`: number of the argument to be printed.
- `min(number)`: the minimum width of the printed argument
- `max(number)`: the maximum width of the printed argument
- `left`: align printing on the left of the minimum space
- `right`: align printing on the right of the minimum space
- `fill(character)`: the fill character for the minimum space (default: 0 for numbers, ' ' for text)
- `precision(number)`: the number of digits after the floating point (only affecting float and double)
- `base(number)`: the base of the number to print
- `bin, oct, dec, hex`: short for base of 2, 8, 10, 16 respectivly (hex used for hex-float as well)
- `sci`: scientific notation for floats
- `sign`: always print the sign with a number
- `upper`: upper case for hex characters and printing strings in uppercase
- `lower`: lower case for hex characters and printing strings in lowercase

Multiple specifiers can be combined with a `:` (e.g. `{min(20):hex:fill(*)}` )
If the specifier is not useful for the argument it will be ignored, repeating
specifiers will be overridden.

On error the function returns a negative integer describing the error.
On success the written characters.

## How it works

The library has defined an index for ever supported type.
C11's `_Generic` macro maps the specified type to the appropriate index
(starting at 1) and a macro puts a pair of `{index, argument}` behind the
format string of the function.
As the function itself is a C99 variadic macro and there are 16 versions
defined to make it recursive. This number can easily be bumped up.
After converting every passed argument to a pair a 0 is added to mark the
last argument. The variadic function `safe_print_implementation` is then
scanning all pairs until it reaches 0 and builds an array out of all
provided arguments. This makes it possible to use positional specifiers as
well.

## Customization:

Put the mentioned `#defines` and `typedefs` before including the header and
the function definitions after.


#### Use your own file API:

    #define SAFE_PRINT_USE_OWN_FILE_OUTPUT
    typedef YOUR_OWN_FILE_TYPE SafePrintFileTyp;
    #define SafePrintStdOut YOU_OWN_STANDARD_OUT
    static void safe_print_output_character(SafePrintContext *context, char c);
    static void safe_print_output_string(SafePrintContext *context, char const *str, size_t length);


#### Use your own integer conversion:

    #define SAFE_PRINT_USE_OWN_INTEGER_CONVERSION
    static SafePrintStringRef safe_print_convert_signed_to_string(char *buffer, sp_s32 size, sp_s64 signed_number, sp_s32 base, sp_s32 uppercase, sp_b32 keep_sign);
    static SafePrintStringRef safe_print_convert_unsigned_to_string(char *buffer, sp_s32 size, sp_u64 number, sp_s32 base, sp_s32 uppercase);


#### Use your own float conversion:

    #define SAFE_PRINT_USE_OWN_FLOAT_CONVERSION
    static SafePrintStringRef safe_print_convert_double_to_string(char *buffer, sp_s32 size, sp_r64 number, sp_s32 precision, sp_b32 scientific, sp_b32 hex, sp_b32 uppercase, sp_b32 keep_sign);


#### Make the function print a descriptive error message:

    #define SAFE_PRINT_DEBUG


## TODO

- implement a `vprintf` like mechanism to forward args to other functions
- make a seperate file for formatting into strings as well
- shorter format specifiers, maybe > and < for min and max, etc...
- the error codes are not correct or not appropriate sometimes
- refactor some of the code to make it more readable and shorter
- remove magic numbers
- remove some unnecessary functions
