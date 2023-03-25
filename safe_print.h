/******************************************************************************
 * License:
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 *
 * ----------------------------------------------------------------------------
 *
 *
 * Explanation:
 *
 * This header is trying to provide a better solution to C's printf-like
 * functions.
 *
 * The format parser is probably a bit overkill, but I had fun building it that
 * way. So far I have no intention to make any performance analysis or changes.
 * This is more a proof of concept and not a meaningful or complete
 * implementation.
 *
 * Unfortunately the library is only available under C11. Support for
 * variadic macros and _Generic is needed for everything to work.
 * Also to count the number of variadic arguments an extension is needed for
 * GCC. You have to compile with -std=gnu11 instead of -std=c11 for it to work.
 * On MSVC the behavior is the default.
 *
 * As you probably know those are not type-safe and you need to manually
 * specify how the next argument has to be interpreted. Modern compilers
 * have some checks to make sure that all format specifiers match the provided
 * arguments, but that is still tedious and not all platforms use the same
 * specifiers for their types. (e.g. long can be 32 or 64 bits) Or don't even
 * support all specifiers. (see size_t and %zu)
 * The function uses a python like syntax with {} as a placeholder for
 * the replacement. But the syntax is different.
 * The library currently only supports 16 arguments to the print routine.
 *
 * ----------------------------------------------------------------------------
 *
 * Usage:
 *
 * This single header file is inspired by Sean Barret's headers.
 * Define SAFE_PRINT_IMPLEMENTATION in a .c file where you want the
 * implementation to be and then #include this header.
 * All other files now only need to include the header without anything else.
 *
 * Call the macro safe_print() with a format string and the required
 * arguments. For example:
 *
 * safe_print("Hello {}!\n", "World");
 *
 * This will print the well known string "Hello World" to stdout.
 * By default the library will use the standart CRT file API.
 *
 * FILE *file = fopen("log.txt", "w");
 * safe_print_file(file, "The answer is: {}\n", 42);
 *
 * This will print "The answer is: 42" to the file log.txt.
 *
 * You can find some more use cases in the examples folder.
 *
 * Format specifiers:
 * - 1 to 16:            number of the argument to be printed.
 * - min(number):        the minimum width of the printed argument
 * - max(number):        the maximum width of the printed argument
 * - left:               align printing on the left of the minimum space
 * - right:              align printing on the right of the minimum space
 * - fill(character):    the fill character for the minimum space (default: 0 for numbers, ' ' for text)
 * - precision(number):  the number of digits after the floating point (only affecting float and double)
 * - base(number):       the base of the number to print
 * - bin, oct, dec, hex: short for base of 2, 8, 10, 16 respectivly (hex used for hex-float as well)
 * - sci:                scientific notation for floats
 * - sign:               always print the sign with a number
 * - upper:              upper case for hex characters and printing strings in uppercase
 * - lower:              lower case for hex characters and printing strings in lowercase
 *
 * Multiple specifiers can be combined with a : (e.g. {min(20):hex:fill(*)} )
 * If the specifier is not useful for the argument it will be ignored, repeating
 * specifiers will be overridden.
 *
 * On error the function returns a negative integer describing the error.
 * On success the written characters.
 *
 * ----------------------------------------------------------------------------
 *
 * How it works:
 *
 * The library has defined an index for ever supported type.
 * C11's _Generic macro maps the specified type to the appropriate index
 * (starting at 1) and a macro puts a pair of {index, argument} behind the
 * format string of the function.
 * As the function itself is a C99 variadic macro and there are 16 versions
 * defined to make it recursive. This number can easily be bumped up.
 * After converting every passed argument to a pair a 0 is added to mark the
 * last argument. The variadic function safe_print_implementation is then
 * scanning all pairs until it reaches 0 and builds an array out of all
 * provided arguments. This makes it possible to use positional specifiers as
 * well.
 *
 * ----------------------------------------------------------------------------
 *
 * Customization:
 *
 * Put the mentioned #defines and typedefs before including the header and
 * the function definitions after.
 *
 *
 * Use your own file API:
 *
 * #define SAFE_PRINT_USE_OWN_FILE_OUTPUT
 * typedef YOUR_OWN_FILE_TYPE SafePrintFileTyp;
 * #define SafePrintStdOut YOU_OWN_STANDARD_OUT
 * static void safe_print_output_character(SafePrintContext *context, char c);
 * static void safe_print_output_string(SafePrintContext *context, char const *str, size_t length);
 *
 *
 * Use your own integer conversion:
 *
 * #define SAFE_PRINT_USE_OWN_INTEGER_CONVERSION
 * static SafePrintStringRef safe_print_convert_signed_to_string(char *buffer, sp_s32 size, sp_s64 signed_number, sp_s32 base, sp_s32 uppercase, sp_b32 keep_sign);
 * static SafePrintStringRef safe_print_convert_unsigned_to_string(char *buffer, sp_s32 size, sp_u64 number, sp_s32 base, sp_s32 uppercase);
 *
 *
 * Use your own float conversion:
 *
 * #define SAFE_PRINT_USE_OWN_FLOAT_CONVERSION
 * static SafePrintStringRef safe_print_convert_double_to_string(char *buffer, sp_s32 size, sp_r64 number, sp_s32 precision, sp_b32 scientific, sp_b32 hex, sp_b32 uppercase, sp_b32 keep_sign);
 *
 *
 * Make the function print a descriptive error message:
 *
 * #define SAFE_PRINT_DEBUG
 *
 *
 * ----------------------------------------------------------------------------
 *
 * Todo:
 *
 * - implement a vprintf like mechanism to forward args to other functions
 * - make a seperate file for formatting into strings as well
 * - shorter format specifiers, maybe > and < for min and max, etc...
 * - the error codes are not correct or not appropriate sometimes
 * - refactor some of the code to make it more readable and shorter
 * - remove magic numbers
 * - remove some unnecessary functions
 *
 * ***************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif


#if !defined(SAFE_PRINT_USE_OWN_FILE_OUTPUT)
#include <stdio.h>

typedef FILE* SafePrintFileType;
#define SafePrintStdOut stdout
#endif

int safe_print_implementation(SafePrintFileType handle, char const *fmt, ...);


enum {
    SP_ERROR_TOO_MANY_ARGUMENTS          = -1,
    SP_ERROR_POSITIONAL_ARG_OUT_OF_RANGE = -2,
    SP_ERROR_UNKNOWN_FORMAT_SPECIFIER    = -3,
    SP_ERROR_MISSING_BRACE               = -4,
};

enum {
    SAFE_PRINT_CHAR = 1,
    SAFE_PRINT_UCHAR,
    SAFE_PRINT_SHORT,
    SAFE_PRINT_USHORT,
    SAFE_PRINT_INT,
    SAFE_PRINT_UINT,
    SAFE_PRINT_LONG,
    SAFE_PRINT_ULONG,
    SAFE_PRINT_LONGLONG,
    SAFE_PRINT_ULONGLONG,
    SAFE_PRINT_FLOAT,
    SAFE_PRINT_DOUBLE,
    SAFE_PRINT_CHAR_PTR,
    SAFE_PRINT_VOID_PTR
};

/*
 * A generic macro to simply get the needed type info for the variadic function.
 * This step would not be necessary if int types where consistent.
 * But long is either 32 or 64 bit depending on the Platform. So we need this for conversion.
 */
#define SAFE_PRINT_ARG_INDEX(type) _Generic((type),	\
char:			SAFE_PRINT_CHAR,	\
unsigned char:		SAFE_PRINT_UCHAR,	\
short:			SAFE_PRINT_SHORT,	\
unsigned short:		SAFE_PRINT_USHORT,	\
int:			SAFE_PRINT_INT,		\
unsigned int:		SAFE_PRINT_INT,		\
long:			SAFE_PRINT_LONG,	\
unsigned long:		SAFE_PRINT_ULONG,	\
long long:		SAFE_PRINT_LONGLONG,	\
unsigned long long:	SAFE_PRINT_ULONGLONG,	\
float:			SAFE_PRINT_FLOAT,	\
double:			SAFE_PRINT_DOUBLE,	\
char const*:		SAFE_PRINT_CHAR_PTR,	\
char*:			SAFE_PRINT_CHAR_PTR,	\
void const*:		SAFE_PRINT_VOID_PTR,	\
void*:			SAFE_PRINT_VOID_PTR	\
)

/*
 * As the variadic macro mechanism of C is of quite limited use, as you can't do any recursion,
 * we have to specify the number of recursions beforehand.
 * If 16 arguments are not enough you need to make this list longer and append to the VA_NUM macro.
 *
 * IMPORTANT: Don't forget to also increase the size of the array in the SafePrintContext!
 */
#define SAFE_PRINT_ARG_0()
#define SAFE_PRINT_ARG_1(expr)       SAFE_PRINT_ARG_INDEX(expr), (expr),
#define SAFE_PRINT_ARG_2(expr, ...)  SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_1(__VA_ARGS__)
#define SAFE_PRINT_ARG_3(expr, ...)  SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_2(__VA_ARGS__)
#define SAFE_PRINT_ARG_4(expr, ...)  SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_3(__VA_ARGS__)
#define SAFE_PRINT_ARG_5(expr, ...)  SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_4(__VA_ARGS__)
#define SAFE_PRINT_ARG_6(expr, ...)  SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_5(__VA_ARGS__)
#define SAFE_PRINT_ARG_7(expr, ...)  SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_6(__VA_ARGS__)
#define SAFE_PRINT_ARG_8(expr, ...)  SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_7(__VA_ARGS__)
#define SAFE_PRINT_ARG_9(expr, ...)  SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_8(__VA_ARGS__)
#define SAFE_PRINT_ARG_10(expr, ...) SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_9(__VA_ARGS__)
#define SAFE_PRINT_ARG_11(expr, ...) SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_10(__VA_ARGS__)
#define SAFE_PRINT_ARG_12(expr, ...) SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_11(__VA_ARGS__)
#define SAFE_PRINT_ARG_13(expr, ...) SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_12(__VA_ARGS__)
#define SAFE_PRINT_ARG_14(expr, ...) SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_13(__VA_ARGS__)
#define SAFE_PRINT_ARG_15(expr, ...) SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_14(__VA_ARGS__)
#define SAFE_PRINT_ARG_16(expr, ...) SAFE_PRINT_ARG_INDEX(expr), (expr), SAFE_PRINT_ARG_15(__VA_ARGS__)

#define SAFE_PRINT_VA_NUM_2(X, X16, X15, X14, X13, X12, X11, X10, X9, X8, X7, X6, X5, X4, X3, X2, X1, N, ...) N
#define SAFE_PRINT_VA_NUM(...) SAFE_PRINT_VA_NUM_2(0, ## __VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define SAFE_PRINT_ARG_N3(N, ...) SAFE_PRINT_ARG_ ## N(__VA_ARGS__)
#define SAFE_PRINT_ARG_N2(N, ...) SAFE_PRINT_ARG_N3(N, __VA_ARGS__)
#define SAFE_PRINT_ARG_N(...)     SAFE_PRINT_ARG_N2(SAFE_PRINT_VA_NUM(__VA_ARGS__), __VA_ARGS__)


#define safe_print(fmt, ...) safe_print_implementation(SafePrintStdOut, (fmt), SAFE_PRINT_ARG_N(__VA_ARGS__) 0)
#define safe_print_file(file, fmt, ...) safe_print_implementation((file), (fmt), SAFE_PRINT_ARG_N(__VA_ARGS__) 0)


#if defined(SAFE_PRINT_IMPLEMENTATION)

#include <stdarg.h>
#include <stdint.h>


#define SAFE_PRINT_BASE(value) (value ? value : 10)

#if defined(SAFE_PRINT_DEBUG)
#define SAFE_PRINT_DEBUG_ERROR_LOCATION(context, location) (context)->error_location = (location)
#else
#define SAFE_PRINT_DEBUG_ERROR_LOCATION(context, location)
#endif


typedef int32_t  sp_s32;
typedef uint32_t sp_u32;
typedef int64_t  sp_s64;
typedef uint64_t sp_u64;
typedef double   sp_r64;
typedef int      sp_b32;

enum {
    sp_false,
    sp_true
};


enum {
    SAFE_PRINT_I32 = 1,
    SAFE_PRINT_U32,
    SAFE_PRINT_I64,
    SAFE_PRINT_U64,
    SAFE_PRINT_R64,
    SAFE_PRINT_CHR,
    SAFE_PRINT_STR,
    SAFE_PRINT_PTR,
};

static int TypeLookupTable[] = {
    0,
    SAFE_PRINT_I32,
    SAFE_PRINT_U32,
    SAFE_PRINT_I32,
    SAFE_PRINT_U32,
    SAFE_PRINT_I32,
    SAFE_PRINT_U32,
    sizeof(long) > sizeof(int) ? SAFE_PRINT_I64 : SAFE_PRINT_I32,
    sizeof(unsigned long) > sizeof(unsigned int) ? SAFE_PRINT_U64 : SAFE_PRINT_U32,
    sizeof(long long) > sizeof(int) ? SAFE_PRINT_I64 : SAFE_PRINT_I32,
    sizeof(unsigned long long) > sizeof(unsigned int) ? SAFE_PRINT_U64 : SAFE_PRINT_U32,
    SAFE_PRINT_R64,
    SAFE_PRINT_R64,
    SAFE_PRINT_STR,
    SAFE_PRINT_STR,
    SAFE_PRINT_PTR,
    SAFE_PRINT_PTR
};

typedef struct SafePrintStringRef {
    char const *data;
    int length;
} SafePrintStringRef;


typedef struct SafePrintFormatArg {
    int kind;
    union {
        sp_s32 s32;
        sp_u32 u32;
        sp_s64 s64;
        sp_u64 u64;
        sp_r64 r64;
        char const* str;
        void const* ptr;
    };
} SafePrintFormatArg;

enum {
    SP_FI_ALIGN_DEFAULT,
    SP_FI_ALIGN_LEFT,
    SP_FI_ALIGN_RIGHT
};
enum {
    SP_FI_DEFAULT_CASE,
    SP_FI_LOWER_CASE,
    SP_FI_UPPER_CASE,
};
typedef struct SafePrintFormatInfo {
    sp_s32 arg_index;
    sp_s32 min;
    sp_s32 max;
    sp_s32 precision;
    sp_s32 base;
    sp_s32 alignment;
    sp_b32 scientific;
    sp_b32 sign;
    sp_u32 char_case;
    sp_u32 fill;
} SafePrintFormatInfo;


enum {
    SP_FT_UNKOWN,
    SP_FT_END_OF_INPUT,
    SP_FT_NUMBER,
    SP_FT_STRING,
    SP_FT_KEYWORD_MIN,
    SP_FT_KEYWORD_MAX,
    SP_FT_KEYWORD_LEFT,
    SP_FT_KEYWORD_RIGHT,
    SP_FT_KEYWORD_FILL,
    SP_FT_KEYWORD_PREC,
    SP_FT_KEYWORD_BASE,
    SP_FT_KEYWORD_SCI,
    SP_FT_KEYWORD_SIGN,
    SP_FT_KEYWORD_UPPER,
    SP_FT_KEYWORD_LOWER,
    SP_FT_BASE_BIN,
    SP_FT_BASE_OCT,
    SP_FT_BASE_DEC,
    SP_FT_BASE_HEX,
    SP_FT_SEPERATOR,
    SP_FT_OPENING_PAREN,
    SP_FT_CLOSING_PAREN,
    SP_FT_OPENING_BRACE,
    SP_FT_CLOSING_BRACE,
};
typedef struct SafePrintFormatToken {
    sp_u32 kind;
    union {
        sp_s32 number;
        sp_u32 character;
        SafePrintStringRef string;
    };
#if defined(SAFE_PRINT_DEBUG)
    sp_s32 location;
#endif // defined(SAFE_PRINT_DEBUG)
} SafePrintFormatToken;


typedef struct SafePrintContext {
    char const *fmt_start;
    char const *fmt;
    
    SafePrintFileType file;
    
    sp_s32 written;
    sp_s32 error;
    
#if defined(SAFE_PRINT_DEBUG)
    sp_s32 error_location;
#endif // defined(SAFE_PRINT_DEBUG)
    
    sp_s32 current_index;
    sp_s32 arg_count;
    SafePrintFormatArg args[16];
} SafePrintContext;



static sp_s64 safe_print_cstring_length(char const *str) {
    sp_s64 length = 0;
    while (str[0]) {
        length += 1;
        str += 1;
    }
    
    return length;
}

static char safe_print_to_lower(char c) {
    if (c >= 'A' && c <= 'Z') return c + 32;
    
    return c;
}

static char safe_print_to_upper(char c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    
    return c;
}

static sp_s32 safe_print_compare_string(SafePrintStringRef token, char const *str, sp_s32 length) {
    if (token.length != length) return sp_false;
    
    for (sp_s32 i = 0; i < length; i += 1) {
        if (token.data[i] != str[i]) return sp_false;
    }
    
    return sp_true;
}

#if !defined(SAFE_PRINT_USE_OWN_INTEGER_CONVERSION) && !defined(SAFE_PRINT_USE_OWN_FLOAT_CONVERSION)

static void safe_print_append_buffer(char *buffer, sp_s32 *size, sp_u32 c) {
    buffer[*size] = c;
    *size += 1;
}

static char SafePrintCharacterLookup[] = "0123456789abcdefghijklmnopqrstuvwxyz";
static char SafePrintCharacterLookupUppercase[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#endif

#if defined(SAFE_PRINT_USE_OWN_INTEGER_CONVERSION)

static SafePrintStringRef safe_print_convert_signed_to_string(char *buffer, sp_s32 size, sp_s64 signed_number, sp_s32 base, sp_s32 uppercase, sp_b32 keep_sign);
static SafePrintStringRef safe_print_convert_unsigned_to_string(char *buffer, sp_s32 size, sp_u64 number, sp_s32 base, sp_s32 uppercase);

#else

static SafePrintStringRef safe_print_convert_signed_to_string(char *buffer, sp_s32 size, sp_s64 signed_number, sp_s32 base, sp_s32 uppercase, sp_b32 keep_sign) {
    sp_s32 is_negative;
    sp_u64 number;
    
    if (signed_number < 0) {
        is_negative = 1;
        number = -signed_number;
    } else {
        is_negative = 0;
        number = signed_number;
    }
    
    char *lookup = uppercase ? SafePrintCharacterLookupUppercase : SafePrintCharacterLookup;
    
    char *ptr = buffer + (size - 1);
    sp_s32 written = 0;
    do {
        sp_s64 quot = number / base;
        sp_s64 rem  = number % base;
        
        *ptr = lookup[rem];
        ptr -= 1;
        written += 1;
        number = quot;
    } while (number && written < size);
    
    if (is_negative || keep_sign) {
        ptr[0] = is_negative ? '-' : '+';
        written += 1;
    } else {
        ptr += 1;
    }
    
    SafePrintStringRef result = {ptr, written};
    return result;
}

static SafePrintStringRef safe_print_convert_unsigned_to_string(char *buffer, sp_s32 size, sp_u64 number, sp_s32 base, sp_s32 uppercase) {
    char *lookup = uppercase ? SafePrintCharacterLookupUppercase : SafePrintCharacterLookup;
    
    char *ptr = buffer + size;
    sp_s32 written = 0;
    do {
        sp_u64 quot = number / base;
        sp_u64 rem  = number % base;
        
        ptr -= 1;
        *ptr = lookup[rem];
        written += 1;
        number = quot;
    } while (number && written < size);
    
    SafePrintStringRef result = {ptr, written};
    return result;
}

#endif // defined(SAFE_PRINT_USE_OWN_INTEGER_CONVERSION)

#if defined(SAFE_PRINT_USE_OWN_FLOAT_CONVERSION)

static SafePrintStringRef safe_print_convert_double_to_string(char *buffer, sp_s32 size, sp_r64 number, sp_s32 precision, sp_b32 scientific, sp_b32 hex, sp_b32 uppercase, sp_b32 keep_sign);

#else

// Using STB's float conversion for simplicity. For a more elaborate explanation, see the end of the file.

#define stbsp__uint32 unsigned int
#define stbsp__int32 signed int

#ifdef _MSC_VER
#define stbsp__uint64 unsigned __int64
#define stbsp__int64 signed __int64
#else
#define stbsp__uint64 unsigned long long
#define stbsp__int64 signed long long
#endif

#define stbsp__uint16 unsigned short

#define STBSP__SPECIAL 0x7000


static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits);
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value);


// TODO: Check the size of the specified buffer. Currently the function expects it to just be large enough.
static SafePrintStringRef safe_print_convert_double_to_string(char *buffer, sp_s32 size, sp_r64 number, sp_s32 precision, sp_b32 scientific, sp_b32 hex, sp_b32 uppercase, sp_b32 keep_sign) {
    stbsp__int32 pos;
    char const *out;
    char tmp[512];
    stbsp__uint32 tmp_size = 512;
    
    char *lookup = uppercase ? SafePrintCharacterLookupUppercase : SafePrintCharacterLookup;
    
    stbsp__int32 sign = stbsp__real_to_str(&out, &tmp_size, tmp, &pos, number, precision);
    
    sp_s32 written = 0;
    if (sign) safe_print_append_buffer(buffer, &written, '-');
    else if (keep_sign) safe_print_append_buffer(buffer, &written, '+');
    
    if (pos == STBSP__SPECIAL) {
        safe_print_append_buffer(buffer, &written, out[0]);
        safe_print_append_buffer(buffer, &written, out[1]);
        safe_print_append_buffer(buffer, &written, out[2]);
        
        return (SafePrintStringRef){buffer, written};
    }
    
    if (hex) {
        stbsp__uint64 bits;
        stbsp__int32 expo;
        sign = stbsp__real_to_parts((stbsp__int64*)&bits, &expo, number);
        
        if (expo == -1023)
            expo = bits ? -1022 : 0;
        else
            bits |= ((stbsp__uint64)1) << 52;
        
        bits <<= 64 - 56;
        if (precision < 15)
            bits += (((stbsp__uint64)8) << 56) >> (precision * 4);
        
        safe_print_append_buffer(buffer, &written, '0');
        safe_print_append_buffer(buffer, &written, 'x');
        
        safe_print_append_buffer(buffer, &written, lookup[(bits >> 60) & 15]);
        safe_print_append_buffer(buffer, &written, '.');
        
        bits <<= 4;
        while (precision) {
            safe_print_append_buffer(buffer, &written, lookup[(bits >> 60) & 15]);
            bits <<= 4;
            precision -= 1;
        }
        safe_print_append_buffer(buffer, &written, uppercase ? 'P' : 'p');
        
        char exp_buffer[4];
        SafePrintStringRef exp_str = safe_print_convert_signed_to_string(exp_buffer, 4, expo, 10, uppercase, sp_true);
        
        for (sp_s32 i = 0; i < exp_str.length; i += 1) {
            safe_print_append_buffer(buffer, &written, exp_str.data[i]);
        }
    } else if (scientific) {
        sp_s32 exp = pos - 1;
        char exp_buffer[4];
        SafePrintStringRef exp_str = safe_print_convert_signed_to_string(exp_buffer, 4, exp, 10, 0, sp_true);
        
        safe_print_append_buffer(buffer, &written, out[0]);
        tmp_size -= 1;
        out += 1;
        
        safe_print_append_buffer(buffer, &written, '.');
        while (tmp_size && precision) {
            safe_print_append_buffer(buffer, &written, out[0]);
            out += 1;
            tmp_size -= 1;
            precision -= 1;
        }
        while (precision) {
            safe_print_append_buffer(buffer, &written, '0');
            precision -= 1;
        }
        safe_print_append_buffer(buffer, &written, uppercase ? 'E' : 'e');
        for (sp_s32 i = 0; i < exp_str.length; i += 1) {
            safe_print_append_buffer(buffer, &written, exp_str.data[i]);
        }
    } else {
        if (pos == 0) {
            safe_print_append_buffer(buffer, &written, '0');
            safe_print_append_buffer(buffer, &written, '.');
        } else if (pos > 0) {
            while (pos && tmp_size) {
                safe_print_append_buffer(buffer, &written, out[0]);
                out += 1;
                tmp_size -= 1;
                pos -= 1;
            }
            while (pos) {
                safe_print_append_buffer(buffer, &written, '0');
                pos -= 1;
            }
            safe_print_append_buffer(buffer, &written, '.');
        } else {
            safe_print_append_buffer(buffer, &written, '0');
            safe_print_append_buffer(buffer, &written, '.');
            
            while (pos < 0 && precision) {
                safe_print_append_buffer(buffer, &written, '0');
                pos += 1;
                precision -= 1;
            }
        }
        
        while (precision && tmp_size) {
            safe_print_append_buffer(buffer, &written, out[0]);
            out += 1;
            tmp_size -= 1;
            precision -= 1;
        }
        
        while (precision) {
            safe_print_append_buffer(buffer, &written, '0');
            precision -= 1;
        }
    }
    
    return (SafePrintStringRef){buffer, written};
}

#endif // defined(SAFE_PRINT_USE_OWN_FLOAT_CONVERSION)

#if defined(SAFE_PRINT_USE_OWN_FILE_OUTPUT)

static void safe_print_output_character(SafePrintContext *context, char c);
static void safe_print_output_string(SafePrintContext *context, char const *str, size_t length);

#else

static void safe_print_output_character(SafePrintContext *context, char c) {
    if (fputc(c, context->file) == EOF) {
        context->error = sp_true;
    } else {
        context->written += 1;
    }
}

static void safe_print_output_string(SafePrintContext *context, char const *str, size_t length) {
    size_t written = fwrite(str, 1, length, context->file);
    context->written += written;
    if (written != length) {
        context->error = sp_true;
    }
}

#endif // defined(SAFE_PRINT_USE_OWN_FILE_OUTPUT)


static void safe_print_apply_format_info(SafePrintContext *context, SafePrintStringRef str, SafePrintFormatInfo info, char default_fill) {
    sp_s32 space = 0;
    if (str.length < info.min) {
        space = info.min - str.length;
    }
    if (info.max && str.length > info.max)
        str.length = info.max;
    
    sp_s32 align = info.alignment ? info.alignment : SP_FI_ALIGN_RIGHT;
    if (align == SP_FI_ALIGN_RIGHT) {
        while (space) {
            safe_print_output_character(context, info.fill ? info.fill : default_fill);
            space -= 1;
        }
    }
    safe_print_output_string(context, str.data, str.length);
    if (align == SP_FI_ALIGN_LEFT) {
        while (space) {
            safe_print_output_character(context, info.fill ? info.fill : default_fill);
            space -= 1;
        }
    }
}

static void safe_print_apply_format_info_to_string(SafePrintContext *context, SafePrintStringRef str, SafePrintFormatInfo info, char default_fill) {
    sp_s32 space = 0;
    if (str.length < info.min) {
        space = info.min - str.length;
    }
    if (info.max && str.length > info.max)
        str.length = info.max;
    
    sp_s32 align = info.alignment ? info.alignment : SP_FI_ALIGN_LEFT;
    if (align == SP_FI_ALIGN_RIGHT) {
        while (space) {
            safe_print_output_character(context, info.fill ? info.fill : default_fill);
            space -= 1;
        }
    }
    
    if (info.char_case == SP_FI_UPPER_CASE) {
        for (sp_s32 i = 0; i < str.length; i += 1) {
            safe_print_output_character(context, safe_print_to_upper(str.data[i]));
        }
    } else if (info.char_case == SP_FI_LOWER_CASE) {
        for (sp_s32 i = 0; i < str.length; i += 1) {
            safe_print_output_character(context, safe_print_to_lower(str.data[i]));
        }
    } else {
        safe_print_output_string(context, str.data, str.length);
    }
    
    if (align == SP_FI_ALIGN_LEFT) {
        while (space) {
            safe_print_output_character(context, info.fill ? info.fill : default_fill);
            space -= 1;
        }
    }
    
}

static void safe_print_format_arg(SafePrintContext *context, SafePrintFormatInfo info) {
    SafePrintFormatArg *arg = &context->args[info.arg_index];
    
    switch (arg->kind) {
        case SAFE_PRINT_I32: {
            char buffer[128];
            SafePrintStringRef str = safe_print_convert_signed_to_string(buffer, 128, arg->s32, SAFE_PRINT_BASE(info.base), info.char_case == SP_FI_UPPER_CASE, info.sign);
            
            safe_print_apply_format_info(context, str, info, '0');
        } break;
        
        case SAFE_PRINT_U32: {
            char buffer[128];
            SafePrintStringRef str = safe_print_convert_unsigned_to_string(buffer, 128, arg->u32, SAFE_PRINT_BASE(info.base), info.char_case == SP_FI_UPPER_CASE);
            
            safe_print_apply_format_info(context, str, info, '0');
        } break;
        
        case SAFE_PRINT_I64: {
            char buffer[128];
            SafePrintStringRef str = safe_print_convert_signed_to_string(buffer, 128, arg->s64, SAFE_PRINT_BASE(info.base), info.char_case == SP_FI_UPPER_CASE, info.sign);
            
            safe_print_apply_format_info(context, str, info, '0');
        } break;
        
        case SAFE_PRINT_U64: {
            char buffer[128];
            SafePrintStringRef str = safe_print_convert_unsigned_to_string(buffer, 128, arg->u64, SAFE_PRINT_BASE(info.base), info.char_case == SP_FI_UPPER_CASE);
            
            safe_print_apply_format_info(context, str, info, '0');
        } break;
        
        case SAFE_PRINT_R64: {
            char buffer[512];
            SafePrintStringRef str = safe_print_convert_double_to_string(buffer, 512, arg->r64, info.precision ? info.precision : 6, info.scientific, info.base == 16 ? sp_true : sp_false, info.char_case == SP_FI_UPPER_CASE, info.sign);
            
            safe_print_apply_format_info(context, str, info, '0');
        } break;
        
        case SAFE_PRINT_STR: {
            SafePrintStringRef str;
            str.data = arg->str;
            str.length = safe_print_cstring_length(str.data);
            
            safe_print_apply_format_info_to_string(context, str, info, ' ');
        } break;
        
        case SAFE_PRINT_PTR: {
            char buffer[128];
            SafePrintStringRef str = safe_print_convert_signed_to_string(buffer, 128, arg->s32, 16, sp_true, sp_false);
            
            safe_print_apply_format_info(context, str, info, '0');
        } break;
    }
}

static sp_b32 safe_print_is_digit(char c) {
    return c >= '0' && c <= '9';
}

static sp_b32 safe_print_is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static void safe_print_report_error(SafePrintContext *context, sp_u32 kind, char const *msg) {
    context->error = kind;
    
#if defined(SAFE_PRINT_DEBUG)
    safe_print_output_character(context, '\n');
    safe_print_output_character(context, '\n');
    
    printf("Error in format string: %s\n", msg);
    
    char const *fmt = context->fmt_start;
    while (fmt[0]) {
        if (fmt[0] == '\n') safe_print_output_character(context, ' ');
        else safe_print_output_character(context, fmt[0]);
        
        fmt += 1;
    }
    safe_print_output_character(context, '\n');
    for (sp_s32 i = 0; i < context->error_location; i += 1) {
        safe_print_output_character(context, '-');
    }
    safe_print_output_character(context, '^');
    safe_print_output_character(context, '\n');
#endif
}

static SafePrintFormatToken safe_print_next_token(SafePrintContext *context) {
    SafePrintFormatToken token = {0};
    
#if defined(SAFE_PRINT_DEBUG)
    token.location = context->fmt - context->fmt_start;
#endif // defined(SAFE_PRINT_DEBUG)
    
    while (context->fmt[0]) {
        switch (context->fmt[0]) {
            case '{': {
                context->fmt += 1;
                token.kind = SP_FT_OPENING_BRACE;
                return token;
            } break;
            
            case '}': {
                context->fmt += 1;
                token.kind = SP_FT_CLOSING_BRACE;
                return token;
            } break;
            
            case '(': {
                context->fmt += 1;
                token.kind = SP_FT_OPENING_PAREN;
                return token;
            } break;
            
            case ')': {
                context->fmt += 1;
                token.kind = SP_FT_CLOSING_PAREN;
                return token;
            } break;
            
            case ' ': case '\n': case '\r': case '\t': { // skip whitespaces
                context->fmt += 1;
            } break;
            
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': {
                while (safe_print_is_digit(context->fmt[0])) {
                    token.number *= 10;
                    token.number += context->fmt[0] - '0';
                    context->fmt += 1;
                }
                token.kind = SP_FT_NUMBER;
                return token;
            } break;
            
            case ':': {
                context->fmt += 1;
                token.kind = SP_FT_SEPERATOR;
                return token;
            } break;
            
            case '\0': {
                token.kind = SP_FT_END_OF_INPUT;
                return token;
            } break;
            
            default: {
                char const *str = context->fmt;
                sp_s32 length = 0;
                
                if (!safe_print_is_letter(context->fmt[0])) {
                    context->fmt += 1;
                    token.kind = SP_FT_UNKOWN;
                    return token;
                }
                
                while (safe_print_is_letter(context->fmt[0])) {
                    length += 1;
                    context->fmt += 1;
                }
                
                token.kind = SP_FT_STRING;
                token.string.data = str;
                token.string.length = length;
                
                
                switch (str[0]) {
                    case 'm': {
                        if (length == 3 && str[1] == 'i' && str[2] == 'n')
                            token.kind = SP_FT_KEYWORD_MIN;
                        else if (length == 3 && str[1] == 'a' && str[2] == 'x')
                            token.kind = SP_FT_KEYWORD_MAX;
                    } break;
                    
                    case 'f': {
                        if (safe_print_compare_string(token.string, "fill", 4))
                            token.kind = SP_FT_KEYWORD_FILL;
                    } break;
                    
                    case 'p': {
                        if (safe_print_compare_string(token.string, "precision", 9))
                            token.kind = SP_FT_KEYWORD_PREC;
                    } break;
                    
                    case 'b': {
                        if (length == 3 && str[1] == 'i' && str[2] == 'n')
                            token.kind = SP_FT_BASE_BIN;
                        else if (length == 4 && str[1] == 'a' && str[2] == 's' && str[3] == 'e')
                            token.kind = SP_FT_KEYWORD_BASE;
                    } break;
                    
                    case 'o': {
                        if (length == 3 && str[1] == 'c' && str[2] == 't')
                            token.kind = SP_FT_BASE_OCT;
                    } break;
                    
                    case 'd': {
                        if (length == 3 && str[1] == 'e' && str[2] == 'c')
                            token.kind = SP_FT_BASE_DEC;
                    } break;
                    
                    case 'h': {
                        if (length == 3 && str[1] == 'e' && str[2] == 'x')
                            token.kind = SP_FT_BASE_HEX;
                    } break;
                    
                    case 'l': {
                        if (length == 4 && str[1] == 'e' && str[2] == 'f' && str[3] == 't')
                            token.kind = SP_FT_KEYWORD_LEFT;
                        else if (safe_print_compare_string(token.string, "lower", 5))
                            token.kind = SP_FT_KEYWORD_LOWER;
                    } break;
                    
                    case 'u': {
                        if (safe_print_compare_string(token.string, "upper", 5))
                            token.kind = SP_FT_KEYWORD_UPPER;
                    } break;
                    
                    case 'r': {
                        if (safe_print_compare_string(token.string, "right", 5))
                            token.kind = SP_FT_KEYWORD_RIGHT;
                    } break;
                    
                    case 's': {
                        if (length == 3 && str[1] == 'c' && str[2] == 'i')
                            token.kind = SP_FT_KEYWORD_SCI;
                        else if (length == 4 && str[1] == 'i' && str[2] == 'g' && str[3] == 'n')
                            token.kind = SP_FT_KEYWORD_SIGN;
                    } break;
                    
                }
                
                return token;
            }
        }
    }
    token.kind = SP_FT_UNKOWN;
    return token;
}

static SafePrintFormatToken safe_print_consume_next_token(SafePrintContext *context, sp_u32 kind, char const *msg) {
    SafePrintFormatToken token = safe_print_next_token(context);
    if (token.kind != kind) {
        SAFE_PRINT_DEBUG_ERROR_LOCATION(context, token.location);
        safe_print_report_error(context, SP_ERROR_UNKNOWN_FORMAT_SPECIFIER , msg);
    }
    
    return token;
}

static sp_b32 safe_print_parse_format_function(SafePrintContext *context, SafePrintFormatToken *out) {
    SafePrintFormatToken opening = safe_print_next_token(context);
    if (opening.kind != SP_FT_OPENING_PAREN) {
        SAFE_PRINT_DEBUG_ERROR_LOCATION(context, opening.location);
        safe_print_report_error(context, SP_ERROR_UNKNOWN_FORMAT_SPECIFIER , "Missing ( after format specifier.");
        return sp_false;
    }
    *out = safe_print_next_token(context);
    
    SafePrintFormatToken closing = safe_print_next_token(context);
    if (closing.kind != SP_FT_CLOSING_PAREN) {
        SAFE_PRINT_DEBUG_ERROR_LOCATION(context, opening.location);
        safe_print_report_error(context, SP_ERROR_UNKNOWN_FORMAT_SPECIFIER , "Missing closing ).");
        return sp_false;
    }
    
    return sp_true;
}

enum {
    SP_PFS_OK,
    SP_PFS_ERROR,
    SP_PFS_ESCAPED_BRACE
};
static sp_s32 safe_print_parse_format_specifier(SafePrintContext *context, SafePrintFormatInfo *out) {
    SafePrintFormatInfo info = {0};
    info.arg_index = -1;
    
    SafePrintFormatToken opening_brace = safe_print_next_token(context);
    if (opening_brace.kind != SP_FT_OPENING_BRACE) {
        SAFE_PRINT_DEBUG_ERROR_LOCATION(context, opening_brace.location);
        safe_print_report_error(context, SP_ERROR_UNKNOWN_FORMAT_SPECIFIER , "Implementation error. Wrong call to safe_print_parse_format_specifier.");
        return SP_PFS_ERROR;
    }
    if (context->fmt[0] == '{') {
        context->fmt += 1;
        return SP_PFS_ESCAPED_BRACE;
    }
    
    SafePrintFormatToken token;
    
    if (context->fmt[0] != '}') {
        do {
            token = safe_print_next_token(context);
            switch (token.kind) {
                case SP_FT_NUMBER: {
                    if (token.number < 0 || token.number > context->arg_count) {
                        SAFE_PRINT_DEBUG_ERROR_LOCATION(context, token.location);
                        safe_print_report_error(context, SP_ERROR_POSITIONAL_ARG_OUT_OF_RANGE , "Bad argument index.");
                        return SP_PFS_ERROR;
                    }
                    info.arg_index = token.number - 1;
                } break;
                
                case SP_FT_KEYWORD_MIN: {
                    SafePrintFormatToken token;
                    if (!safe_print_parse_format_function(context, &token))
                        return SP_PFS_ERROR;
                    
                    if (token.kind != SP_FT_NUMBER) {
                        SAFE_PRINT_DEBUG_ERROR_LOCATION(context, token.location);
                        safe_print_report_error(context, SP_ERROR_UNKNOWN_FORMAT_SPECIFIER , "Expected number in min specifier.");
                        return SP_PFS_ERROR;
                    }
                    info.min = token.number;
                } break;
                
                case SP_FT_KEYWORD_MAX: {
                    safe_print_consume_next_token(context, SP_FT_OPENING_PAREN, "Missing ( after max specifier.");
                    SafePrintFormatToken token = safe_print_consume_next_token(context, SP_FT_NUMBER, "Expected number inside max specifier.");
                    info.max = token.number;
                    safe_print_consume_next_token(context, SP_FT_CLOSING_PAREN, "Missing ) after max specifier.");
                } break;
                
                case SP_FT_KEYWORD_FILL: {
                    safe_print_consume_next_token(context, SP_FT_OPENING_PAREN, "Missing ( after fill specifier.");
                    info.fill = context->fmt[0];
                    context->fmt += 1;
                    safe_print_consume_next_token(context, SP_FT_CLOSING_PAREN, "Missing ) after fill specifier.");
                } break;
                
                case SP_FT_KEYWORD_PREC: {
                    safe_print_consume_next_token(context, SP_FT_OPENING_PAREN, "Missing ( after fill specifier.");
                    SafePrintFormatToken token = safe_print_consume_next_token(context, SP_FT_NUMBER, "Expected number inside precision specifier.");
                    info.precision = token.number;
                    safe_print_consume_next_token(context, SP_FT_CLOSING_PAREN, "Missing ) after fill specifier.");
                } break;
                
                case SP_FT_KEYWORD_LEFT: { info.alignment = SP_FI_ALIGN_LEFT; } break;
                case SP_FT_KEYWORD_RIGHT: { info.alignment = SP_FI_ALIGN_RIGHT; } break;
                
                case SP_FT_KEYWORD_BASE: {
                    safe_print_consume_next_token(context, SP_FT_OPENING_PAREN, "Missing ( after fill specifier.");
                    SafePrintFormatToken token = safe_print_consume_next_token(context, SP_FT_NUMBER, "Expected number inside precision specifier.");
                    info.base = token.number;
                    safe_print_consume_next_token(context, SP_FT_CLOSING_PAREN, "Missing ) after fill specifier.");
                } break;
                
                case SP_FT_KEYWORD_SCI: { info.scientific = sp_true; } break;
                case SP_FT_KEYWORD_SIGN: { info.sign = sp_true; } break;
                
                case SP_FT_KEYWORD_LOWER: { info.char_case = SP_FI_LOWER_CASE; } break;
                case SP_FT_KEYWORD_UPPER: { info.char_case = SP_FI_UPPER_CASE; } break;
                
                case SP_FT_BASE_BIN: { info.base = 2; } break;
                case SP_FT_BASE_OCT: { info.base = 8; } break;
                case SP_FT_BASE_DEC: { info.base = 10; } break;
                case SP_FT_BASE_HEX: { info.base = 16; } break;
                
                case SP_FT_STRING: {
                    SAFE_PRINT_DEBUG_ERROR_LOCATION(context, token.location);
                    safe_print_report_error(context, SP_ERROR_UNKNOWN_FORMAT_SPECIFIER , "Unknown format specifier.");
                    return SP_PFS_ERROR;
                } break;
                
                case SP_FT_END_OF_INPUT: {
                    SAFE_PRINT_DEBUG_ERROR_LOCATION(context, token.location);
                    safe_print_report_error(context, SP_ERROR_MISSING_BRACE , "Reached end of format string, missing }.");
                    return SP_PFS_ERROR;
                } break;
                
                case SP_FT_UNKOWN: {
                    SAFE_PRINT_DEBUG_ERROR_LOCATION(context, opening_brace.location);
                    safe_print_report_error(context, SP_ERROR_UNKNOWN_FORMAT_SPECIFIER , "Missing } in format specifier.");
                    return SP_PFS_ERROR;
                } break;
            }
            
            token = safe_print_next_token(context);
        } while (token.kind == SP_FT_SEPERATOR);
        
    } else {
        token = safe_print_next_token(context);
    }
    
    if (token.kind != SP_FT_CLOSING_BRACE) {
        SAFE_PRINT_DEBUG_ERROR_LOCATION(context, opening_brace.location);
        safe_print_report_error(context, SP_ERROR_MISSING_BRACE , "Missing } in format specifier.");
        return SP_PFS_ERROR;
    }
    
    if (info.arg_index == -1) {
        info.arg_index = context->current_index;
        if (info.arg_index == context->arg_count) {
            SAFE_PRINT_DEBUG_ERROR_LOCATION(context, opening_brace.location);
            safe_print_report_error(context, SP_ERROR_TOO_MANY_ARGUMENTS , "Not enough print arguments for this format specifier");
            return SP_PFS_ERROR;
        }
        context->current_index += 1;
    }
    
    *out = info;
    return SP_PFS_OK;
}

int safe_print_implementation(SafePrintFileType handle, char const *fmt, ...) {
    SafePrintContext context = {0};
    context.fmt_start = fmt;
    context.fmt = fmt;
    context.file = handle;
    
    va_list args;
    va_start(args, fmt);
    
    int arg_type = va_arg(args, int);
    while (arg_type) {
        SafePrintFormatArg arg = {0};
        
        int arg_kind = TypeLookupTable[arg_type];
        
        switch (arg_kind) {
            case SAFE_PRINT_I32: {
                arg.kind = SAFE_PRINT_I32;
                arg.s32 = va_arg(args, sp_s32);
            } break;
            
            case SAFE_PRINT_U32: {
                arg.kind = SAFE_PRINT_U32;
                arg.u32 = va_arg(args, sp_u32);
            } break;
            
            case SAFE_PRINT_I64: {
                arg.kind = SAFE_PRINT_I64;
                arg.s64 = va_arg(args, sp_s64);
            } break;
            
            case SAFE_PRINT_U64: {
                arg.kind = SAFE_PRINT_U64;
                arg.u64 = va_arg(args, sp_u64);
            } break;
            
            case SAFE_PRINT_R64: {
                arg.kind = SAFE_PRINT_R64;
                arg.r64 = va_arg(args, sp_r64);
            } break;
            
            case SAFE_PRINT_STR: {
                arg.kind = SAFE_PRINT_STR;
                arg.str = va_arg(args, char const*);
            } break;
            
            case SAFE_PRINT_PTR: {
                arg.kind = SAFE_PRINT_PTR;
                arg.ptr = va_arg(args, void const*);
            } break;
        }
        
        context.args[context.arg_count] = arg;
        context.arg_count += 1;
        
        arg_type = va_arg(args, int);
    }
    
    va_end(args);
    
    while (context.fmt[0]) {
        if (context.error) return context.error;
        
        if (context.fmt[0] == '{') {
            SafePrintFormatInfo info;
            sp_s32 status = safe_print_parse_format_specifier(&context, &info);
            if (status == SP_PFS_ERROR) {
                return SP_ERROR_UNKNOWN_FORMAT_SPECIFIER ;
            } else if (status == SP_PFS_ESCAPED_BRACE) {
                safe_print_output_character(&context, '{');
            } else {
                safe_print_format_arg(&context, info);
            }
        } else if (context.fmt[0] == '}') {
            if (context.fmt[1] == '}') {
                safe_print_output_character(&context, '}');
                context.fmt += 2;
            } else {
                SAFE_PRINT_DEBUG_ERROR_LOCATION(&context, context.fmt - context.fmt_start);
                safe_print_report_error(&context, SP_ERROR_MISSING_BRACE , "stray } in format string.");
                context.fmt += 1;
            }
        } else {
            safe_print_output_character(&context, context.fmt[0]);
            context.fmt += 1;
        }
    }
    
    return context.written;
}



/**********************************************************************************************************************
 *
 * Here I just copied the entire floating point conversion routines from stb_sprintf of Sean Barret's
 * STB Nothings libs.
 * I choose to use his code because it is in the public domain and that is what I intend for my library as well.
 * If you want to use a more sophisticated algorithm you can always define your own floating point conversion
 * routine with SAFE_PRINT_USE_OWN_FLOAT_CONVERSION and use e.g. Grisu or Ryu.
 * But for simplicity and accessibility I will stick to STB for now.
 *
 * Source: https://github.com/nothings/stb
 *
 *********************************************************************************************************************/


#if !defined(SAFE_PRINT_USE_OWN_FLOAT_CONVERSION)




static struct
{
    short temp; // force next field to be 2-byte aligned
    char pair[201];
} stbsp__digitpair =
{
    0,
    "00010203040506070809101112131415161718192021222324"
        "25262728293031323334353637383940414243444546474849"
        "50515253545556575859606162636465666768697071727374"
        "75767778798081828384858687888990919293949596979899"
};

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define STBSP__COPYFP(dest, src)			\
{							\
int cn;						\
for (cn = 0; cn < 8; cn++)			\
((char *)&dest)[cn] = ((char *)&src)[cn];\
}

// get float info
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value)
{
    double d;
    stbsp__int64 b = 0;
    
    // load value and round at the frac_digits
    d = value;
    
    STBSP__COPYFP(b, d);
    
    *bits = b & ((((stbsp__uint64)1) << 52) - 1);
    *expo = (stbsp__int32)(((b >> 52) & 2047) - 1023);
    
    return (stbsp__int32)((stbsp__uint64) b >> 63);
}

static double const stbsp__bot[23] = {
    1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
    1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017, 1e+018, 1e+019, 1e+020, 1e+021, 1e+022
};
static double const stbsp__negbot[22] = {
    1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011,
    1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019, 1e-020, 1e-021, 1e-022
};
static double const stbsp__negboterr[22] = {
    -5.551115123125783e-018,  -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022, 4.5251888174113741e-023,
    4.5251888174113739e-024,  -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028,  2.0113352370744385e-029,
    -3.0373745563400371e-030, 1.1806906454401013e-032,  -7.7705399876661076e-032, 2.0902213275965398e-033,  -7.1542424054621921e-034, -7.1542424054621926e-035,
    2.4754073164739869e-036,  5.4846728545790429e-037,  9.2462547772103625e-038,  -4.8596774326570872e-039
};
static double const stbsp__top[13] = {
    1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299
};
static double const stbsp__negtop[13] = {
    1e-023, 1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299
};
static double const stbsp__toperr[13] = {
    8388608,
    6.8601809640529717e+028,
    -7.253143638152921e+052,
    -4.3377296974619174e+075,
    -1.5559416129466825e+098,
    -3.2841562489204913e+121,
    -3.7745893248228135e+144,
    -1.7356668416969134e+167,
    -3.8893577551088374e+190,
    -9.9566444326005119e+213,
    6.3641293062232429e+236,
    -5.2069140800249813e+259,
    -5.2504760255204387e+282
};
static double const stbsp__negtoperr[13] = {
    3.9565301985100693e-040,  -2.299904345391321e-063,  3.6506201437945798e-086,  1.1875228833981544e-109,
    -5.0644902316928607e-132, -6.7156837247865426e-155, -2.812077463003139e-178,  -5.7778912386589953e-201,
    7.4997100559334532e-224,  -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
    8.0970921678014997e-317
};

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
static stbsp__uint64 const stbsp__powten[20] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000,
    10000000000000000000U
};
#define stbsp__tento19th ((stbsp__uint64)1000000000000000000)
#else
static stbsp__uint64 const stbsp__powten[20] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000ULL,
    100000000000ULL,
    1000000000000ULL,
    10000000000000ULL,
    100000000000000ULL,
    1000000000000000ULL,
    10000000000000000ULL,
    100000000000000000ULL,
    1000000000000000000ULL,
    10000000000000000000ULL
};
#define stbsp__tento19th (1000000000000000000ULL)
#endif

#define stbsp__ddmulthi(oh, ol, xh, yh)                            \
{                                                               \
double ahi = 0, alo, bhi = 0, blo;                           \
stbsp__int64 bt;                                             \
oh = xh * yh;                                                \
STBSP__COPYFP(bt, xh);                                       \
bt &= ((~(stbsp__uint64)0) << 27);                           \
STBSP__COPYFP(ahi, bt);                                      \
alo = xh - ahi;                                              \
STBSP__COPYFP(bt, yh);                                       \
bt &= ((~(stbsp__uint64)0) << 27);                           \
STBSP__COPYFP(bhi, bt);                                      \
blo = yh - bhi;                                              \
ol = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
}

#define stbsp__ddtoS64(ob, xh, xl)          \
{                                        \
double ahi = 0, alo, vh, t;           \
ob = (stbsp__int64)xh;                \
vh = (double)ob;                      \
ahi = (xh - vh);                      \
t = (ahi - xh);                       \
alo = (xh - (ahi - t)) - (vh + t);    \
ob += (stbsp__int64)(ahi + alo + xl); \
}

#define stbsp__ddrenorm(oh, ol) \
{                            \
double s;                 \
s = oh + ol;              \
ol = ol - (s - oh);       \
oh = s;                   \
}

#define stbsp__ddmultlo(oh, ol, xh, xl, yh, yl) ol = ol + (xh * yl + xl * yh);

#define stbsp__ddmultlos(oh, ol, xh, yl) ol = ol + (xh * yl);

static void stbsp__raise_to_power10(double *ohi, double *olo, double d, stbsp__int32 power) // power can be -323 to +350
{
    double ph, pl;
    if ((power >= 0) && (power <= 22)) {
        stbsp__ddmulthi(ph, pl, d, stbsp__bot[power]);
    } else {
        stbsp__int32 e, et, eb;
        double p2h, p2l;
        
        e = power;
        if (power < 0)
            e = -e;
        et = (e * 0x2c9) >> 14; /* %23 */
        if (et > 13)
            et = 13;
        eb = e - (et * 23);
        
        ph = d;
        pl = 0.0;
        if (power < 0) {
            if (eb) {
                --eb;
                stbsp__ddmulthi(ph, pl, d, stbsp__negbot[eb]);
                stbsp__ddmultlos(ph, pl, d, stbsp__negboterr[eb]);
            }
            if (et) {
                stbsp__ddrenorm(ph, pl);
                --et;
                stbsp__ddmulthi(p2h, p2l, ph, stbsp__negtop[et]);
                stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__negtop[et], stbsp__negtoperr[et]);
                ph = p2h;
                pl = p2l;
            }
        } else {
            if (eb) {
                e = eb;
                if (eb > 22)
                    eb = 22;
                e -= eb;
                stbsp__ddmulthi(ph, pl, d, stbsp__bot[eb]);
                if (e) {
                    stbsp__ddrenorm(ph, pl);
                    stbsp__ddmulthi(p2h, p2l, ph, stbsp__bot[e]);
                    stbsp__ddmultlos(p2h, p2l, stbsp__bot[e], pl);
                    ph = p2h;
                    pl = p2l;
                }
            }
            if (et) {
                stbsp__ddrenorm(ph, pl);
                --et;
                stbsp__ddmulthi(p2h, p2l, ph, stbsp__top[et]);
                stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__top[et], stbsp__toperr[et]);
                ph = p2h;
                pl = p2l;
            }
        }
    }
    stbsp__ddrenorm(ph, pl);
    *ohi = ph;
    *olo = pl;
}

// given a float value, returns the significant bits in bits, and the position of the
//   decimal point in decimal_pos.  +/-INF and NAN are specified by special values
//   returned in the decimal_pos parameter.
// frac_digits is absolute normally, but if you want from first significant digits (got %g and %e), or in 0x80000000
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits)
{
    double d;
    stbsp__int64 bits = 0;
    stbsp__int32 expo, e, ng, tens;
    
    d = value;
    STBSP__COPYFP(bits, d);
    expo = (stbsp__int32)((bits >> 52) & 2047);
    ng = (stbsp__int32)((stbsp__uint64) bits >> 63);
    if (ng)
        d = -d;
    
    if (expo == 2047) // is nan or inf?
    {
        *start = (bits & ((((stbsp__uint64)1) << 52) - 1)) ? "NaN" : "Inf";
        *decimal_pos = STBSP__SPECIAL;
        *len = 3;
        return ng;
    }
    
    if (expo == 0) // is zero or denormal
    {
        if (((stbsp__uint64) bits << 1) == 0) // do zero
        {
            *decimal_pos = 1;
            *start = out;
            out[0] = '0';
            *len = 1;
            return ng;
        }
        // find the right expo for denormals
        {
            stbsp__int64 v = ((stbsp__uint64)1) << 51;
            while ((bits & v) == 0) {
                --expo;
                v >>= 1;
            }
        }
    }
    
    // find the decimal exponent as well as the decimal bits of the value
    {
        double ph, pl;
        
        // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
        tens = expo - 1023;
        tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);
        
        // move the significant bits into position and stick them into an int
        stbsp__raise_to_power10(&ph, &pl, d, 18 - tens);
        
        // get full as much precision from double-double as possible
        stbsp__ddtoS64(bits, ph, pl);
        
        // check if we undershot
        if (((stbsp__uint64)bits) >= stbsp__tento19th)
            ++tens;
    }
    
    // now do the rounding in integer land
    frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
    if ((frac_digits < 24)) {
        stbsp__uint32 dg = 1;
        if ((stbsp__uint64)bits >= stbsp__powten[9])
            dg = 10;
        while ((stbsp__uint64)bits >= stbsp__powten[dg]) {
            ++dg;
            if (dg == 20)
                goto noround;
        }
        if (frac_digits < dg) {
            stbsp__uint64 r;
            // add 0.5 at the right position and round
            e = dg - frac_digits;
            if ((stbsp__uint32)e >= 24)
                goto noround;
            r = stbsp__powten[e];
            bits = bits + (r / 2);
            if ((stbsp__uint64)bits >= stbsp__powten[dg])
                ++tens;
            bits /= r;
        }
        noround:;
    }
    
    // kill long trailing runs of zeros
    if (bits) {
        stbsp__uint32 n;
        for (;;) {
            if (bits <= 0xffffffff)
                break;
            if (bits % 1000)
                goto donez;
            bits /= 1000;
        }
        n = (stbsp__uint32)bits;
        while ((n % 1000) == 0)
            n /= 1000;
        bits = n;
        donez:;
    }
    
    // convert to string
    out += 64;
    e = 0;
    for (;;) {
        stbsp__uint32 n;
        char *o = out - 8;
        // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
        if (bits >= 100000000) {
            n = (stbsp__uint32)(bits % 100000000);
            bits /= 100000000;
        } else {
            n = (stbsp__uint32)bits;
            bits = 0;
        }
        while (n) {
            out -= 2;
            *(stbsp__uint16 *)out = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
            n /= 100;
            e += 2;
        }
        if (bits == 0) {
            if ((e) && (out[0] == '0')) {
                ++out;
                --e;
            }
            break;
        }
        while (out != o) {
            *--out = '0';
            ++e;
        }
    }
    
    *decimal_pos = tens;
    *start = out;
    *len = e;
    return ng;
}

#endif // !defined(SAFE_PRINT_USE_OWN_FLOAT_CONVERSION)

#ifdef __cplusplus
}
#endif

#endif

