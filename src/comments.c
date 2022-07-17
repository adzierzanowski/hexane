#include "comments.h"


// Adds a comment at `pos`
void h_comment_add(struct h_state_t *state, char *comment, int pos) {
  char *com = calloc(strlen(comment) + 1, sizeof (char));
  strcpy(com, comment);

  h_comment_remove(state, pos);

  state->combuf[pos] = com;
}

// Removes a comment at `pos` if it exists.
void h_comment_remove(struct h_state_t *state, int pos) {
  char *com = state->combuf[pos];
  if (!com) {
    return;
  }

  free(com);
  state->combuf[pos] = NULL;
  h_msg(state, "Removed a comment at position %d.", pos);
}
