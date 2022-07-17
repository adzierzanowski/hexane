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

char **h_strsplit(char *str, const char *delim, size_t *splitsz) {
  char *s = calloc(strlen(str) + 1, sizeof (char));
  strcpy(s, str);

  char **split = NULL;
  *splitsz = 0;

  char *tok = strtok(s, delim);
  while (tok) {
    (*splitsz)++;
    split = realloc(split, sizeof (char *) * *splitsz);
    split[*splitsz-1] = calloc(strlen(tok) + 1, sizeof (char));
    strcpy(split[*splitsz-1], tok);
    tok = strtok(NULL, delim);
  }

  free(s);
  return split;
}

void h_strsplit_free(char **split, size_t splitsz) {
  for (int i = 0; i < splitsz; i++) {
    free(split[i]);
  }

  free(split);
}
