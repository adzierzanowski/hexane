#include "mark.h"


void h_mark_bytes(struct h_state_t *state) {
  struct h_select_t *sel = h_selection_next(state, true);

  if (!sel) {
    state->markbuf[state->cursor_pos] = state->color;
    return;
  }

  while (sel) {
    struct h_select_t s = h_selection_sorted(sel);

    for (int i = s.start; i <= s.end; i++) {
      state->markbuf[i] = state->color;
    }

    sel = h_selection_next(state, false);
  }
}

void h_color_eyedrop(struct h_state_t *state) {
  state->color = state->markbuf[state->cursor_pos];
  state->color_pos = state->color;
  h_msg(state, "Color set to %d.", state->color);
}
