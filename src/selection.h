#ifndef H_SELECTION
#define H_SELECTION

#include <stdbool.h>
#include <stdlib.h>

#include "state.h"


void h_select(struct h_state_t *state);
void h_selection_update(struct h_state_t *state);
void h_selection_clear(struct h_state_t *state);
bool h_selection_includes(struct h_state_t *state, int pos);

#endif
