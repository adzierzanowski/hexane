#include "cursor.h"


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

  if (pos >= lim || pos < state->offset) {
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
  state->searchpos = state->cursor_pos;
}

// Moves the cursor relative to the current position by `diff` bytes.
void h_cursor_move(struct h_state_t *state, int diff) {
  int new_pos = state->cursor_pos + diff;

  if (new_pos < 0 || new_pos >= state->bufsz) {
    return;
  }

  state->cursor_pos = new_pos;

  h_cursor_validate(state);
  state->searchpos = state->cursor_pos;
}
