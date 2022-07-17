#ifndef H_SELECTION
#define H_SELECTION

#include <stdbool.h>
#include <stdlib.h>

#include "state.h"


// Make a selection.
void h_select(struct h_state_t *state);

// Update selection state.
void h_selection_update(struct h_state_t *state);

// Clear the selection.
void h_selection_clear(struct h_state_t *state);

// Check if a position is selected.
bool h_selection_includes(struct h_state_t *state, int pos);

// Get an ordered selection.
struct h_select_t h_selection_sorted(struct h_select_t *sel);

// Iterate over selections.
struct h_select_t *h_selection_next(struct h_state_t *state, bool reset);

#endif
