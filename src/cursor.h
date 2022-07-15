#ifndef H_CURSOR
#define H_CURSOR

#include "state.h"


// Validates the current position of the cursor and doesn't let it move out of
// the bounds
void h_cursor_validate(struct h_state_t *state);

// Moves the cursor to an arbitrary position
void h_cursor_goto(struct h_state_t *state, int new_pos);

// Moves the cursor relative to the current position
void h_cursor_move(struct h_state_t *state, int diff);

#endif
