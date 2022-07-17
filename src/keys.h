#ifndef KEYS_H
#define KEYS_H

#include "cmdline.h"
#include "cursor.h"
#include "mark.h"
#include "state.h"


// Handles the keys when in the cmdline mode
void h_key_handle_cmd(int key, struct h_state_t *state);

// Handles the keys in general
void h_key_handle(int key, struct h_state_t *state);

#endif
