#include "selection.h"


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
