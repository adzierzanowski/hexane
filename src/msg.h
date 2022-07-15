#ifndef H_MSG
#define H_MSG

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "state.h"


// Sets the current message
void h_msg(struct h_state_t *state, const char *fmt, ...);

// Clears the current message
void h_msg_clear(struct h_state_t *state);

#endif
