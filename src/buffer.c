#include "buffer.h"


// Loads the `buf` of size `bufsz` bytes into the `state`'s buffer.
void h_load_buffer(struct h_state_t *state, uint8_t *buf, size_t bufsz) {
  state->buffer = calloc(bufsz, sizeof (uint8_t));
  state->markbuf = calloc(bufsz, sizeof (uint8_t));
  state->combuf = calloc(bufsz, sizeof (char *));
  state->searchposbuf = calloc(bufsz, sizeof (uint8_t));
  state->searchbuf = calloc(bufsz, sizeof (uint8_t));
  state->bufsz = bufsz;
  memcpy(state->buffer, buf, bufsz);
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

  strncpy(state->fname, fname, H_BUFSZ-1);
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

// This function sets selected bytes (or at cursor pos if there's no selection)
// to `val`.
void h_buffer_set_bytes(struct h_state_t *state, uint8_t val) {
  struct h_select_t *sel = h_selection_next(state, true);

  if (!sel) {
    state->buffer[state->cursor_pos] = val;
    return;
  }

  while (sel) {
    struct h_select_t s = h_selection_sorted(sel);

    for (int i = s.start; i <= s.end; i++) {
      state->buffer[i] = val;
    }

    sel = h_selection_next(state, false);
  }
}

// This function sets bytes' values relative to their current ones.
void h_buffer_set_bytes_rel(struct h_state_t *state, int diff) {
  struct h_select_t *sel = h_selection_next(state, true);

  if (!sel) {
    state->buffer[state->cursor_pos] += diff;
    return;
  }

  while (sel) {
    struct h_select_t s = h_selection_sorted(sel);

    for (int i = s.start; i <= s.end; i++) {
      state->buffer[i] += diff;
    }

    sel = h_selection_next(state, false);
  }
}

// This function increments selected bytes' values.
void h_buffer_increment(struct h_state_t *state) {
  h_buffer_set_bytes_rel(state, 1);
}

// This function decrements selected bytes' values.
void h_buffer_decrement(struct h_state_t *state) {
  h_buffer_set_bytes_rel(state, -1);
}
