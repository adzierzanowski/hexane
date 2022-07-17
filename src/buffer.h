#ifndef H_BUFFER
#define H_BUFFER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msg.h"
#include "state.h"
#include "selection.h"


// Loads a buffer
void h_load_buffer(struct h_state_t *state, uint8_t *buf, size_t bufsz);

// Opens a file and calls h_load_buffer
void h_edit_file(struct h_state_t *state, const char *fname);

// Saves the buffer to a file
void h_save_file(struct h_state_t *state, const char *fname);

// Set byte(s) relatively to their own value.
void h_buffer_set_bytes_rel(struct h_state_t *state, int diff);

// Set byte(s) to a value.
void h_buffer_set_bytes(struct h_state_t *state, uint8_t val);

// Increment currently selected bytes.
void h_buffer_increment(struct h_state_t *state);

// Decrement currently selected bytes.
void h_buffer_decrement(struct h_state_t *state);

#endif
