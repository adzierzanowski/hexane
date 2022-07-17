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
    struct h_select_t *next = ptr->next;
    free(ptr);
    ptr = next;
  }
  state->cursel = NULL;
  state->cursel_size = 0;

  ptr = state->selections;
  while (ptr != NULL) {
    struct h_select_t *next = ptr->next;
    free(ptr);
    ptr = next;
  }

  state->selections = NULL;
  state->selsize = 0;
}

// Returns `true` if the byte in the `pos` position is selected.
// Otherwise, returns `false`.
bool h_selection_includes(struct h_state_t *state, int pos) {
  struct h_select_t *sel = h_selection_next(state, true);

  while (sel != NULL) {
    struct h_select_t s = h_selection_sorted(sel);

    if (pos >= s.start && pos <= s.end) {
      return true;
    }

    sel = h_selection_next(state, false);
  }

  return false;
}

// Returns a copy of `sel` with sorted `start` and `end` fields.
struct h_select_t h_selection_sorted(struct h_select_t *sel) {
  struct h_select_t out;
  if (sel->end > sel->start) {
    out.start = sel->start;
    out.end = sel->end;
  } else {
    out.start = sel->end;
    out.end = sel->start;
  }
  out.next = sel->next;

  return out;
}

// This function is an "iterator" which chains two linked lists of selections.
// It returns a pointer to an element of these lists.
// If `reset` is set to `true`, the function chooses a selection which is
// probably not NULL. When the ptr becomes NULL, the function switches to the
// other list and returns pointers to that list's elements.
struct h_select_t *h_selection_next(struct h_state_t *state, bool reset) {
  static struct h_select_t *ptr = NULL;

  // Are we in the list selected at the reset?
  static bool in_first = true;

  // Did we begin with the current selections list at the reset?
  static bool started_with_cursel = false;

  if (reset) {
    if (state->selections) {
      // We've got some elements in this list, so let's start with it.
      ptr = state->selections;
      started_with_cursel = false;
    } else {
      ptr = state->cursel;
      started_with_cursel = true;
    }

    in_first = true;
    return ptr;
  }

  ptr = ptr->next;

  // We've reached the NULL in the first list. Make a switch to the other one.
  if (!ptr && in_first) {
    if (started_with_cursel) {
      ptr = state->selections;
    } else {
      ptr = state->cursel;
    }
    in_first = false;
  }

  return ptr;
}
