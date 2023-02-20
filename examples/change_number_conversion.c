#define SAFE_PRINT_IMPLEMENTATION

#define SAFE_PRINT_USE_OWN_INTEGER_CONVERSION
#define SAFE_PRINT_USE_OWN_FLOAT_CONVERSION

#include "../safe_print.h"
#define print safe_print


#if defined(_WIN32) || defined(WIN32)
char const* format_s64_sign = "%+lld";
char const* format_s64 = "%lld";
char const* format_u64 = "%llu";
#else
char const* format_s64_sign = "%+ld";
char const* format_s64 = "%ld";
char const* format_u64 = "%lu";
#endif

static SafePrintStringRef safe_print_convert_signed_to_string(char *buffer, sp_s32 size, sp_s64 signed_number, sp_s32 base, sp_s32 uppercase, sp_b32 keep_sign) {
    SafePrintStringRef result;
    result.data = buffer;
    // NOTE: I don't want to write code for the base and uppercase stuff. You should use something that supports this easily
    if (keep_sign)
        result.length = snprintf(buffer, size, format_s64_sign, signed_number);
    else
        result.length = snprintf(buffer, size, format_s64, signed_number);

    return result;
}

static SafePrintStringRef safe_print_convert_unsigned_to_string(char *buffer, sp_s32 size, sp_u64 number, sp_s32 base, sp_s32 uppercase) {
    SafePrintStringRef result;
    result.data = buffer;
    // NOTE: Same like above
    result.length = snprintf(buffer, size, format_u64, number);
    return result;
}


static SafePrintStringRef safe_print_convert_double_to_string(char *buffer, sp_s32 size, sp_r64 number, sp_s32 precision, sp_b32 scientific, sp_b32 hex, sp_b32 uppercase, sp_b32 keep_sign) {
    SafePrintStringRef result;
    result.data = buffer;
    // NOTE: Same like above
    result.length = snprintf(buffer, size, "%f", number);
    return result;
}


int main(int argc, char **argv) {
    print("{}\n", 42);

    print("{}\n", 42.5);
}

