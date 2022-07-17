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
#define H_STATUSLINESZ 9

enum h_seltype_t {
  H_SELTYPE_NONE,
  H_SELTYPE_NORMAL,
  H_SELTYPE_SINGLE,
  H_SELTYPE_RECT
};

// A struct representing selection. It's a linked list containing ranges of
// positions (a start and end position, inclusive).
struct h_select_t {
  struct h_select_t *next;
  int start;
  int end;
};

struct h_query_t {
  uint8_t *terms;
  size_t terms_sz;
};

struct h_state_t {
  // Currently loaded bytes
  uint8_t *buffer;

  // Color-marking information
  uint8_t *markbuf;

  uint8_t *searchbuf;
  uint8_t *searchposbuf;
  size_t search_result_count;
  int searchpos;

  // Comments
  char **combuf;

  // Length of the main buffer
  size_t bufsz;

  // Current cursor position (index of the buffer)
  int cursor_pos;


  // Applied selections (non-modifiable until cleared)
  struct h_select_t *selections;

  // Currently modified selection
  struct h_select_t *cursel;

  // Mode of the current selection
  enum h_seltype_t cursel_type;

  // Total number of selected bytes in the applied selections list
  size_t selsize;

  // Total number of selected bytes in the current selection list
  size_t cursel_size;


  // Number of displayed columns
  int cols;

  // Number of displayed lines
  int lines;

  // Offset of the displayed portion of the buffer (in bytes)
  int offset;

  // Current TTY width and height
  uint16_t tcw, tch;

  // Selcted marking color
  uint8_t fg;

  // Current position of the color picker
  int color_pos;

  // Currently selected marking color
  int color;

  // Number of the key which was pressed last
  int last_key;


  // The command buffer
  char cmdbuf[H_CMDBUFSZ];

  // The position of currently entered command
  int cmdpos;

  // Message buffer
  char msgbuf[H_MSGBUFSZ];

  // Currently loaded file's name
  char fname[H_BUFSZ];


  struct h_query_t **queries;


  // Is the command line active
  bool cmdline;

  // Is the ASCII representation displayed
  bool ascii;

  // Are the column numbers visible
  bool header;

  // Does the viewport size change on TTY resize
  bool autosize;

  // Are the numbers in the statusline displayed hexadecimally
  bool hex;

  // Is color picker active
  bool color_picker;
};


// Initializes state
void h_state_init(struct h_state_t *state);

// Gets the current limit (the number of last byte (+1) that should be displayed)
int h_getlim(struct h_state_t *state);

#endif
