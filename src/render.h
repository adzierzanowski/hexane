#ifndef H_RENDER
#define H_RENDER

#include <ctype.h>
#include <stdio.h>

#include "io.h"
#include "msg.h"
#include "selection.h"
#include "state.h"


// Gets an ANSI escape code depending on the current state
// if the byte in the `pos` position shouldn't be formatted,
// the function will return NULL
const char *h_getansi(struct h_state_t *state, int pos);

// Renders the current line's ASCII representation
void h_render_ascii(struct h_state_t *state, int pos);

// Renders the status line(s)
void h_render_statusline(struct h_state_t *state);

// Renders the buffer
void h_render(struct h_state_t *state);

// Updates the viewport state depending on the current TTY size
void h_tty_update(struct h_state_t *state);

#endif
