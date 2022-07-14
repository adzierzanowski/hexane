#ifndef HEX_H
#define HEX_H

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "io.h"
#include "util.h"

#define H_CMDBUFSZ 0x100
#define H_MSGBUFSZ 0x100
#define H_BUFSZ 0x100
#define H_FBUFSZ 0x10000
#define H_STATUSLINESZ 7


struct h_state_t {
  uint8_t *buffer;
  size_t bufsz;
  int cursor_pos;

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

// FILE/BUFFER HANDLERS

// Loads a buffer
void h_load_buffer(struct h_state_t *state, uint8_t *buf, size_t bufsz);

// Opens a file and calls h_load_buffer
void h_edit_file(struct h_state_t *state, const char *fname);

// Saves the buffer to a file
void h_save_file(struct h_state_t *state, const char *fname);


// STATE

// Initializes state
void h_state_init(struct h_state_t *state);

// Gets the current limit (the number of last byte (+1) that should be displayed)
int h_getlim(struct h_state_t *state);


// MESSAGES

// Sets the current message
void h_msg(struct h_state_t *state, const char *fmt, ...);

// Clears the current message
void h_msg_clear(struct h_state_t *state);


// RENDERING

// Gets an ANSI escape code depending on the current state
// if the byte in the `pos` position shouldn't be formatted,
// the function will return NULL
const char *h_getansi(struct h_state_t *state, int pos);

// Renders the current line's ASCII representation
void h_render_ascii(struct h_state_t *state, int pos);

// Renders the status line(s)
void h_render_statusline(struct h_state_t *state);

// Renders the buffer
void h_render(struct h_state_t *state);

// Updates the viewport state depending on the current TTY size
void h_tty_update(struct h_state_t *state);


// CURSOR MOVEMENT

// Validates the current position of the cursor and doesn't let it move out of
// the bounds
void h_cursor_validate(struct h_state_t *state);

// Moves the cursor to an arbitrary position
void h_cursor_goto(struct h_state_t *state, int new_pos);

// Moves the cursor relative to the current position
void h_cursor_move(struct h_state_t *state, int diff);


// COMMAND LINE

// Handles the command
void h_cmd_handle(struct h_state_t *state);

// Clears the cmdline buffer and exit the cmdline mode
void h_cmd_clear(struct h_state_t *state);


// INPUT HANDLERS

// Handles the keys when in the cmdline mode
void h_key_handle_cmd(int key, struct h_state_t *state);

// Handles the keys in general
void h_key_handle(int key, struct h_state_t *state);


#endif
