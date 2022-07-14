#ifndef H_UTIL
#define H_UTIL

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Matches the string against other strings
// If `str` matches any of the strings, the return value is true
// Otherwise, false
// The argument list is terminated with NULL
bool h_strmatch(const char *str, ...);

#endif
