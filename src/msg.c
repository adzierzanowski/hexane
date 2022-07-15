#include "msg.h"


// Sets the current `state`'s message to the formatted string `fmt`.
void h_msg(struct h_state_t *state, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(state->msgbuf, H_MSGBUFSZ, fmt, args);
  va_end(args);
}

// Clears the current `state`'s message.
void h_msg_clear(struct h_state_t *state) {
  memset(state->msgbuf, 0, H_MSGBUFSZ);
}
