#include "buffer.h"


// Loads the `buf` of size `bufsz` bytes into the `state`'s buffer.
void h_load_buffer(struct h_state_t *state, uint8_t *buf, size_t bufsz) {
  state->buffer = calloc(bufsz, sizeof(uint8_t));
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
