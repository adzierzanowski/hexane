#ifndef H_COMMENTS
#define H_COMMENTS

#include "msg.h"
#include "state.h"

void h_comment_add(struct h_state_t *state, char *comment, int pos);
void h_comment_remove(struct h_state_t *state, int pos);

#endif
