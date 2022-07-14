#include "util.h"


bool h_strmatch(const char *str, ...) {

  va_list args;
  va_start(args, str);

  const char *ptr = va_arg(args, const char *);

  while (ptr != NULL) {
    if (strcmp(str, ptr) == 0) {
      va_end(args);
      return true;
    }

    ptr = va_arg(args, const char *);
  }

  va_end(args);
  return false;
}
