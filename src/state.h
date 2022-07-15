#ifndef H_STATE
#define H_STATE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "io.h"

#define H_CMDBUFSZ 0x100
#define H_MSGBUFSZ 0x100
#define H_BUFSZ 0x100
#define H_FBUFSZ 0x10000
#define H_STATUSLINESZ 7

enum h_seltype_t {
  H_SELTYPE_NONE,
  H_SELTYPE_NORMAL,
  H_SELTYPE_SINGLE,
  H_SELTYPE_RECT
};

struct h_select_t {
  struct h_select_t *next;
  int start;
  int end;
};

struct h_state_t {
  uint8_t *buffer;
  size_t bufsz;
  int cursor_pos;

  struct h_select_t *selections;
  struct h_select_t *cursel;
  enum h_seltype_t cursel_type;
  size_t selsize;
  size_t cursel_size;

  int cols;
  int lines;
  int offset;

  uint16_t tcw, tch;

  int last_key;

  char cmdbuf[H_CMDBUFSZ];
  int cmdpos;
  char msgbuf[H_MSGBUFSZ];
  char fname[H_BUFSZ];

  bool cmdline;
  bool ascii;
  bool header;
  bool autosize;
};


// Initializes state
void h_state_init(struct h_state_t *state);

// Gets the current limit (the number of last byte (+1) that should be displayed)
int h_getlim(struct h_state_t *state);

#endif
