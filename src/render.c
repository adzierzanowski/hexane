#include "render.h"


// Returns an ANSI escape sequence if the byte in the `pos` position
// should be formatted somehow. If it shouldn't, the function will
// return NULL.
const char *h_getansi(struct h_state_t *state, int pos) {
  if (pos == state->cursor_pos) {
    return "\x1b[7m";
  }

  if (h_selection_includes(state, pos)) {
    return "\x1b[38;5;51m";
  }

  return NULL;
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

    const char *ansi = h_getansi(state, i);
    bool printable = isprint(c);

    printf("%s%s%c%s",
      ansi ? ansi : "",
      printable ? "" : "\x1b[38;5;8m",
      printable ? c : '.',
      ansi || !printable ? "\x1b[0m" : "");
  }

  printf("\n");
}

// Renders the status line.
void h_render_statusline(struct h_state_t *state) {
  printf("%s    ", state->hex ? "HEX" : "DEC");

  if (state->hex) {
    printf("CUR %x/%zx    ", state->cursor_pos, state->bufsz);
    printf("KEY %x %c    ",
        state->last_key,
        isprint(state->last_key) && state->last_key != '\n'
          ? state->last_key
          : ' ');
    printf("OFF %x    ", state->offset);
    printf("CMD %s    ", state->cmdline ? "on" : "off");
    printf("LIM %x    ", h_getlim(state));
    printf("TTY %xx%x    ", state->tcw, state->tch);
    printf("SEL %zx    ", state->selsize + state->cursel_size);
    printf("FILE %s", state->fname);

  } else {
    printf("CUR %d/%zu    ", state->cursor_pos, state->bufsz);
    printf("KEY %u %c    ",
        state->last_key,
        isprint(state->last_key) && state->last_key != '\n'
          ? state->last_key
          : ' ');
    printf("OFF %d    ", state->offset);
    printf("CMD %s    ", state->cmdline ? "on" : "off");
    printf("LIM %d    ", h_getlim(state));
    printf("TTY %dx%d    ", state->tcw, state->tch);
    printf("SEL %zu    ", state->selsize + state->cursel_size);
    printf("FILE %s", state->fname);
  }

  printf("\n");

  void *ptr = (void *) &state->buffer[state->cursor_pos];
  uint8_t u8 = state->buffer[state->cursor_pos];
  uint16_t *u16 = (uint16_t *) ptr;
  uint32_t *u32 = (uint32_t *) ptr;
  uint64_t *u64 = (uint64_t *) ptr;

  if (state->hex) {
    printf(
      "u8 %02x    u16 %04x    u32 %08x    u64 %016llx\n", u8, *u16, *u32, *u64);
  } else {
    printf(
      "u8 %3u    u16 %5u    u32 %10u    u64 %20llu\n", u8, *u16, *u32, *u64);
    printf(
      "i8 %3d    i16 %5d    i32 %10d    i64 %20lld\n", u8, *u16, *u32, *u64);
  }

  if (state->cmdline) {
    printf(":%s_\n", state->cmdbuf);
  }

  if (strlen(state->msgbuf) > 0) {
    printf("%s\n", state->msgbuf);
    h_msg_clear(state);
  }
}

// Renders the current `state` of the buffer.
// Calls other rendering functions if needed.
void h_render(struct h_state_t *state) {
  h_tty_cls();
  h_tty_goto(0, 0);

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
      const char *ansi = h_getansi(state, i);

      printf("%s%02x%s ",
        ansi ? ansi : "",
        state->buffer[i],
        ansi ? "\x1b[0m" : "");

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
