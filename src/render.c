#include "render.h"


// Stores an ANSI escape sequence in `buf` if the byte in the `pos` position
// should be formatted somehow. If it shouldn't, the function will
// store an empty string in `buf`.
void h_getansi(struct h_state_t *state, int pos, char *dst) {
  if (pos == state->cursor_pos) {
    sprintf(dst, "\x1b[7m");
    return;
  }

  if (state->searchbuf[pos]) {
    sprintf(dst, "\x1b[48;5;208m\x1b[38;5;255m");
    return;
  }

  if (h_selection_includes(state, pos)) {
    sprintf(dst, "\x1b[48;5;51m");
    return;
  }

  if (state->markbuf[pos] != 0) {
    sprintf(dst, "\x1b[38;5;%dm", state->markbuf[pos]);
    return;
  }


  dst[0] = 0;
}

// Renders the ASCII representation of the current line
// (the one containing the byte in the `pos` position).
void h_render_ascii(struct h_state_t *state, int pos) {
  int start = pos - (pos % state->cols);
  int end = start + state->cols;

  if (end > state->bufsz) {
    end = state->bufsz;
  }

  for (int i = start; i < end; i++) {
    char c = state->buffer[i];

    char ansi[H_BUFSZ] = {0};
    h_getansi(state, i, ansi);
    bool printable = isprint(c);

    printf("%s%s%c%s",
      ansi,
      printable ? "" : "\x1b[38;5;8m",
      printable ? c : '.',
      ansi[0] || !printable ? "\x1b[0m" : "");
  }

  printf("\n");
}

// Renders the status line.
void h_render_statusline(struct h_state_t *state) {
  printf("%s    ", state->hex ? "HEX" : "DEC");

  #define H_GRAY "\x1b[38;5;249m"
  #define H_END "\x1b[0m"

  double ratio = (double) state->cursor_pos / (double) state->bufsz * 100.0;

  if (state->hex) {
    printf("CUR " H_GRAY "%x/%zx (%3.0lf%%)" H_END "    ", state->cursor_pos, state->bufsz, ratio);
    printf("KEY " H_GRAY "%x %c" H_END "    ",
        state->last_key,
        isprint(state->last_key) && state->last_key != '\n'
          ? state->last_key
          : ' ');
    printf("OFF " H_GRAY "%x" H_END "    ", state->offset);
    printf("CMD " H_GRAY "%s" H_END "    ", state->cmdline ? "on" : "off");
    printf("LIM " H_GRAY "%x" H_END "    ", h_getlim(state));
    printf("\x1b[48;5;%dm CLR \x1b[0m    ", state->color);
    printf("SEL " H_GRAY "%zx" H_END "    ", state->selsize + state->cursel_size);
    printf("FILE " H_GRAY "%s" H_END, state->fname);

  } else {
    printf("CUR " H_GRAY "%d/%zu (%3.0lf%%)" H_END "    ", state->cursor_pos, state->bufsz, ratio);
    printf("KEY " H_GRAY "%u %c" H_END "    ",
        state->last_key,
        isprint(state->last_key) && state->last_key != '\n'
          ? state->last_key
          : ' ');
    printf("OFF " H_GRAY "%d" H_END "    ", state->offset);
    printf("CMD " H_GRAY "%s" H_END "    ", state->cmdline ? "on" : "off");
    printf("LIM " H_GRAY "%d" H_END "    ", h_getlim(state));
    printf("\x1b[48;5;%dm CLR \x1b[0m    ", state->color);
    printf("SEL " H_GRAY "%zu" H_END "    ", state->selsize + state->cursel_size);
    printf("FILE " H_GRAY "%s" H_END, state->fname);
  }

  printf("\n");

  if (state->bufsz > 0) {
    void *ptr = (void *) &state->buffer[state->cursor_pos];
    uint8_t u8 = state->buffer[state->cursor_pos];
    uint16_t *u16 = (uint16_t *) ptr;
    uint32_t *u32 = (uint32_t *) ptr;
    uint64_t *u64 = (uint64_t *) ptr;

    if (state->hex) {
      printf(
        "u8 " H_GRAY "%02x" H_END "    u16 " H_GRAY "%04x" H_END "    u32 " H_GRAY "%08x" H_END "    u64 " H_GRAY "%016llx" H_END "\n", u8, *u16, *u32, *u64);
    } else {
      printf(
        "u8 " H_GRAY "%3u" H_END "    u16 " H_GRAY "%5u" H_END "    u32 " H_GRAY "%10u" H_END "    u64 " H_GRAY "%20llu" H_END "\n", u8, *u16, *u32, *u64);
      printf(
        "i8 " H_GRAY "%3d" H_END"    i16 " H_GRAY "%5d" H_END "    i32 " H_GRAY "%10d" H_END "    i64 " H_GRAY "%20lld" H_END "\n", u8, *u16, *u32, *u64);
    }
  }


  if (state->cmdline) {
    printf(":%s_\n", state->cmdbuf);
  } else if (state->bufsz > 0 && state->combuf[state->cursor_pos]) {
    printf("Comment: %s\n", state->combuf[state->cursor_pos]);
  }

  if (strlen(state->msgbuf) > 0) {
    printf("\x1b[48;5;147m\x1b[1m\x1b[38;5;232m %s \x1b[0m\n", state->msgbuf);
    h_msg_clear(state);
  }
}

// Renders the current `state` of the buffer.
// Calls other rendering functions if needed.
void h_render(struct h_state_t *state) {
  h_tty_cls();
  h_tty_goto(0, 0);

  if (state->color_picker) {
    h_render_color_picker(state);
    return;
  }

  if (state->bufsz == 0) {
    printf("Empty buffer, type `:e FNAME` to edit file FNAME.");

  } else {
    int lim = h_getlim(state);

    if (state->header) {
      printf("\x1b[38;5;208m");
      for (int i = 0; i < state->cols; i++) {
        printf("%02x ", i);
      }
      printf("\x1b[0m\n");
    }

    for (int i = state->offset; i < lim; i++) {
      char ansi[H_BUFSZ] = {0};
      h_getansi(state, i, ansi);

      printf("%s%02x%s ",
        ansi,
        state->buffer[i],
        ansi[0] ? "\x1b[0m" : "");

      if ((i+1) % state->cols == 0) {
        if (state->ascii) {
          printf(" ");
          h_render_ascii(state, i);
        } else {
          printf("\n");
        }
      }
    }
  }

  printf("\n");
  if (h_getlim(state) % state->cols != 0) {
    printf("\n");
  }

  h_render_statusline(state);
}

// Updates the `state` according to the current size of the TTY.
void h_tty_update(struct h_state_t *state) {
  uint32_t ttysz = h_ttysz();
  state->tcw = ttysz >> 16;
  state->tch = ttysz & 0xffff;

  if (state->lines > (state->tch - H_STATUSLINESZ)) {
    state->lines = state->tch - H_STATUSLINESZ;

    if (state->header) {
      state->lines--;
    }
  }

  if (state->autosize) {
    if (state->ascii) {
      state->cols = state->tcw / 4 - 1;
    } else {
      state->cols = state->tcw / 3;
    }

    state->lines = state->tch - H_STATUSLINESZ;
    if (state->header) {
      state->lines--;
    }

    h_render(state);
  }
}

void h_render_color_picker(struct h_state_t *state) {
  h_tty_cls();
  h_tty_goto(0, 0);

  for (int i = 0; i < 0x100; i++) {
    if (state->color_pos == i) {
      printf("\x1b[38;5;51m %3d \x1b[0m ", i);
    } else {
      printf("\x1b[48;5;%dm %3d \x1b[0m ", i, i);
    }

    if ((i+1) % 12 == 0) {
      printf("\n");
    }
  }
}
