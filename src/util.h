#ifndef H_UTIL
#define H_UTIL

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


// Matches the string against other strings
// If `str` matches any of the strings, the return value is true
// Otherwise, false
// The argument list is terminated with NULL
bool h_strmatch(const char *str, ...);

char **h_strsplit(char *str, const char *delim, size_t *splitsz);
void h_strsplit_free(char **split, size_t splitsz);

#endif
