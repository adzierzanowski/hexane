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

void h_load_buffer(struct h_state_t *state, uint8_t *buf, size_t bufsz);
void h_edit_file(struct h_state_t *state, const char *fname);
void h_save_file(struct h_state_t *state, const char *fname);

void h_state_init(struct h_state_t *state);
int h_getlim(struct h_state_t *state);

void h_msg(struct h_state_t *state, const char *fmt, ...);
void h_msg_clear(struct h_state_t *state);

const char *h_getansi(struct h_state_t *state, int pos);
void h_render_ascii(struct h_state_t *state, int pos);
void h_render_statusline(struct h_state_t *state);
void h_render(struct h_state_t *state);
void h_tty_update(struct h_state_t *state);

void h_cursor_validate(struct h_state_t *state);
void h_cursor_goto(struct h_state_t *state, int new_pos);
void h_cursor_move(struct h_state_t *state, int diff);

void h_cmd_clear(struct h_state_t *state);
void h_cmd_handle(struct h_state_t *state);

void h_key_handle_cmd(int key, struct h_state_t *state);
void h_key_handle(int key, struct h_state_t *state);

#endif
