#ifndef H_CMDLINE
#define H_CMDLINE

#include "buffer.h"
#include "msg.h"
#include "render.h"
#include "state.h"
#include "util.h"


// Handles the command
void h_cmd_handle(struct h_state_t *state);

// Clears the cmdline buffer and exit the cmdline mode
void h_cmd_clear(struct h_state_t *state);

#endif
