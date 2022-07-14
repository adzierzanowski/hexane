#include "hex.h"


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

// Loads the `buf` of size `bufsz` bytes into the `state`'s buffer.
void h_load_buffer(struct h_state_t *state, uint8_t *buf, size_t bufsz) {
  state->buffer = calloc(bufsz, sizeof(uint8_t));
  state->bufsz = bufsz;
  memcpy(state->buffer, buf, bufsz);
}

// Initializes the `state` with sensible defaults.
void h_state_init(struct h_state_t *state) {
  state->buffer = NULL;
  state->bufsz = 0;
  state->cursor_pos = 0;

  state->offset = 0;

  uint32_t ttysz = h_ttysz();
  state->tcw = ttysz >> 16;
  state->tch = ttysz & 0xffff;
  state->lines = state->tch - H_STATUSLINESZ;

  state->cols = 2;
  while (state->cols * 3 < state->tcw) {
    state->cols *= 2;
  }
  state->cols /= 2;

  memset(state->cmdbuf, 0, H_CMDBUFSZ);
  state->cmdline = false;
  state->cmdpos = 0;

  memset(state->msgbuf, 0, H_MSGBUFSZ);
  state->ascii = false;
  state->header = false;
  state->autosize = false;

  memset(state->fname, 0, H_BUFSZ);

  state->selections = NULL;
  state->cursel = NULL;
  state->cursel_type = H_SELTYPE_NONE;
  state->selsize = 0;
  state->cursel_size = 0;
}

// Updates the `state` according to the current size of the TTY.
void h_tty_update(struct h_state_t *state) {
  uint32_t ttysz = h_ttysz();
  state->tcw = ttysz >> 16;
  state->tch = ttysz & 0xffff;

  if (state->lines > (state->tch - H_STATUSLINESZ)) {
    state->lines = state->tch - H_STATUSLINESZ;
  }

  if (state->autosize) {
    state->cols = state->tcw / 3;
    state->lines = state->tch - H_STATUSLINESZ;

    h_render(state);
  }
}

// Returns a number of the next byte to the last byte that should be
// displayed.
int h_getlim(struct h_state_t *state) {
  int lim = state->offset + state->cols * state->lines;
  lim = lim < state->bufsz ? lim : state->bufsz;
  return lim;
}

// Sets the current `state`'s message to the formatted string `fmt`.
void h_msg(struct h_state_t *state, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(state->msgbuf, H_MSGBUFSZ, fmt, args);
  va_end(args);
}

// Clears the current `state`'s message.
void h_msg_clear(struct h_state_t *state) {
  memset(state->msgbuf, 0, H_MSGBUFSZ);
}

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
  printf("CUR %d    ", state->cursor_pos);
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
  printf("\n");

  if (state->cmdline) {
    printf(":%s_\n", state->cmdbuf);
  }

  if (strlen(state->msgbuf) > 0) {
    printf("%s\n", state->msgbuf);
    h_msg_clear(state);
  }
}

// Validates the current cursor position. If the position is wrong,
// the function tries to set the cursor in a proper position.
void h_cursor_validate(struct h_state_t *state) {
  int pos = state->cursor_pos;

  if (pos < 0) {
    state->cursor_pos = 0;
  } else if (pos >= state->bufsz) {
    state->cursor_pos = state->bufsz - 1;
  }

  pos = state->cursor_pos;

  int lim = h_getlim(state);

  if (pos > lim || pos < state->offset) {
    int poslim = pos - lim;

    if (poslim > 0 && poslim < state->cols) {
      state->offset += state->cols;
    } else {
      state->offset = state->cursor_pos - (state->cursor_pos % state->cols);
    }

    while (h_getlim(state) - state->offset < state->lines * state->cols) {
      state->offset -= state->cols;
    }

    if (h_getlim(state) < state->cursor_pos) {
      state->offset += state->cols;
    }
  }
}

// Moves the cursor to an arbitrary position `new_pos`.
void h_cursor_goto(struct h_state_t *state, int new_pos) {
  state->cursor_pos = new_pos;
  h_cursor_validate(state);
}

// Moves the cursor relative to the current position by `diff` bytes.
void h_cursor_move(struct h_state_t *state, int diff) {
  int new_pos = state->cursor_pos + diff;

  if (new_pos < 0 || new_pos >= state->bufsz) {
    return;
  }

  state->cursor_pos = new_pos;

  h_cursor_validate(state);
}

// Resets the command line buffer and exits the command line mode.
void h_cmd_clear(struct h_state_t *state) {
  memset(state->cmdbuf, 0, H_CMDBUFSZ);
  state->cmdpos = 0;
  state->cmdline = false;
}

// Reads a file `fname` and calls the buffer loading function.
void h_edit_file(struct h_state_t *state, const char *fname) {
  FILE *f = fopen(fname, "rb");

  if (!f) {
    h_msg(state, "Failed to open file '%s'", fname);
    return;
  }

  uint8_t buf[H_FBUFSZ] = {0};
  size_t nbytes = fread(buf, sizeof (uint8_t), H_FBUFSZ, f);
  fclose(f);

  strncpy(state->fname, fname, H_BUFSZ);
  h_msg(state, "Read %zu bytes from '%s'", nbytes, fname);
  h_load_buffer(state, buf, nbytes);
}

// Writes the current state of the buffer into a file named `fname`.
void h_save_file(struct h_state_t *state, const char *fname) {
  FILE *f = fopen(fname, "wb");

  if (!f) {
    h_msg(state, "Failed to save file '%s'", fname);
    return;
  }

  size_t nbytes = fwrite(state->buffer, sizeof (uint8_t), state->bufsz, f);
  fclose(f);

  h_msg(state, "Written %zu bytes to '%s'", nbytes, fname);
}

// Handles the command passed by the user in the command line mode.
void h_cmd_handle(struct h_state_t *state) {
  char *cmd = state->cmdbuf;

  if (!cmd[0]) {
    h_msg(state, "Empty command.");
    h_cmd_clear(state);
    return;
  }

  if (strcmp(cmd, "q") == 0) {
    exit(0);

  } else if (h_strmatch(cmd, "a", "ascii", NULL)) {
    state->ascii = !state->ascii;
    h_msg(state, "Show ASCII set to %s", state->ascii ? "true" : "false");

  } else if (h_strmatch(cmd, "h", "header", NULL)) {
    state->header = !state->header;
    h_msg(state, "Show header set to %s.", state->header ? "true" : "false");

  } else if (h_strmatch(cmd, "as", "autosize", NULL)) {
    state->autosize = !state->autosize;
    h_msg(state, "Autosize set to %s.", state->autosize ? "true" : "false");

  } else {
    char *tok = strtok(state->cmdbuf, " ");

    if (h_strmatch(tok, "c", "cols", NULL)) {
      tok = strtok(NULL, " ");

      if (tok == NULL) {
        h_msg(state, "Column number: %d", state->cols);
        h_cmd_clear(state);
        return;
      }

      int n = atoi(tok);
      if (n > 0 && n <= 128) {
        state->cols = n;
        h_msg(state, "Column number changed to %d", n);
      } else {
        h_msg(state, "Bad argument for cols.");
      }

    } else if (h_strmatch(tok, "e", "edit", NULL)) {
      tok = strtok(NULL, " ");
      if (tok == NULL) {
        h_msg(state, "No filename provided.");
        h_cmd_clear(state);
        return;
      }

      h_edit_file(state, tok);

    } else if (h_strmatch(tok, "w", "write", NULL)) {
      tok = strtok(NULL, " ");
      if (tok == NULL) {
        if (state->fname[0] != 0) {
          h_save_file(state, tok);
        } else {
          h_msg(state, "No filename provided.");
        }

      } else {
        h_save_file(state, tok);
      }

    } else if (h_strmatch(tok, "l", "lines", NULL)) {
      tok = strtok(NULL, " ");

      if (tok == NULL) {
        h_msg(state, "Line limit: %d", state->lines);
        h_cmd_clear(state);
        return;
      }

      int lines = atoi(tok);

      if (lines > 0 && lines < state->tch) {
        state->lines = lines;
        h_tty_update(state);
        h_msg(state, "Set line limit to %d", state->lines);

      } else {
        h_msg(state, "Bad argument for line limit.");
      }

    } else {
      h_msg(state, "Unknown command: %s", state->cmdbuf);
    }
  }

  h_cmd_clear(state);
}

// Handles the key pressed by the user in the command line mode.
void h_key_handle_cmd(int key, struct h_state_t *state) {
  switch (key) {
    case '\x1b': {
      h_cmd_clear(state);
      break;
    }

    case '\x7f': {
      if (state->cmdpos > 0) {
        state->cmdbuf[--state->cmdpos] = 0;
      }
      break;
    }

    case '\n': {
      h_cmd_handle(state);
      break;
    }

    default: {
      if (state->cmdpos < H_CMDBUFSZ && (isprint(key))) {
        state->cmdbuf[state->cmdpos++] = (char) key;
      }

      break;
    }
  }

  h_render(state);
}

// Handles the key pressed by the user in the normal mode.
void h_key_handle(int key, struct h_state_t *state) {
  state->last_key = key;

  if (state->cmdline) {
    h_key_handle_cmd(key, state);
    return;
  }

  switch (key) {
    case '0':
      h_cursor_move(state, -state->cursor_pos % state->cols);
      break;

    case '$':
      h_cursor_move(state, state->cols - (state->cursor_pos % state->cols) - 1);
      break;

    case 'g':
      h_cursor_goto(state, 0);
       break;

    case 'G':
      h_cursor_goto(state, state->bufsz-1);
      break;

    case 'b':
      h_cursor_move(state, -state->cols / 8);
      break;

    case 'w':
      h_cursor_move(state, state->cols / 8);
      break;

    case 'h':
      h_cursor_move(state, -1);
      break;

    case 'j':
      h_cursor_move(state, state->cols);
      break;

    case 'k':
      h_cursor_move(state, -state->cols);
      break;

    case 'l':
      h_cursor_move(state, 1);
      break;

    case ':':
      state->cmdline = true;
      break;

    case 'v':
      h_select(state);
      break;

    case '\x1b':
      h_selection_clear(state);
      break;

    default:
      break;
  }

  h_selection_update(state);
  h_render(state);
}

// Starts a new selection in the current position
// or applies the current selection.
void h_select(struct h_state_t *state) {
  if (state->cursel == NULL) {
    struct h_select_t *new_selection = malloc(sizeof (struct h_select_t));
    new_selection->start = state->cursor_pos;
    new_selection->end = state->cursor_pos;
    new_selection->next = NULL;
    state->cursel = new_selection;
    state->cursel_size = 1;

  } else {
    if (state->selections == NULL) {
      state->selections = state->cursel;

    } else {
      struct h_select_t *ptr = state->selections;
      while (ptr->next != NULL) {
        ptr = ptr->next;
      }
      ptr->next = state->cursel;

    }

    state->selsize += state->cursel_size;
    state->cursel_size = 0;
    state->cursel = NULL;
  }
}

// Updates the selection state based on the current cursor position.
void h_selection_update(struct h_state_t *state) {
  struct h_select_t *sel = state->cursel;

  if (sel == NULL) {
    return;
  }

  if (state->cursel_type == H_SELTYPE_RECT) {
    // TODO: Not implemented yet

  } else {
    sel->end = state->cursor_pos;
    state->cursel_size = abs(sel->end - sel->start) + 1;
  }
}

// Clears all selections.
void h_selection_clear(struct h_state_t *state) {
  struct h_select_t *ptr = state->cursel;

  while (ptr != NULL) {
    free(ptr);
    ptr = ptr->next;
  }
  state->cursel = NULL;
  state->cursel_size = 0;

  ptr = state->selections;
  while (ptr != NULL) {
    free(ptr);
    ptr = ptr->next;
  }

  state->selections = NULL;
  state->selsize = 0;
}

// Returns `true` if the byte in the `pos` position is selected.
// Otherwise, returns `false`.
bool h_selection_includes(struct h_state_t *state, int pos) {
  struct h_select_t *sel = state->cursel;

  while (sel != NULL) {
    int start = sel->start;
    int end = sel->end;
    if (start > end) {
      start = sel->end;
      end = sel->start;
    }

    if (pos >= start && pos <= end) {
      return true;
    }

    sel = sel->next;
  }

  sel = state->selections;

  while (sel != NULL) {
    int start = sel->start;
    int end = sel->end;
    if (start > end) {
      start = sel->end;
      end = sel->start;
    }

    if (pos >= start && pos <= end) {
      return true;
    }

    sel = sel->next;
  }

  return false;
}
