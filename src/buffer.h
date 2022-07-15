#ifndef H_BUFFER
#define H_BUFFER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msg.h"
#include "state.h"


// Loads a buffer
void h_load_buffer(struct h_state_t *state, uint8_t *buf, size_t bufsz);

// Opens a file and calls h_load_buffer
void h_edit_file(struct h_state_t *state, const char *fname);

// Saves the buffer to a file
void h_save_file(struct h_state_t *state, const char *fname);

#endif
