#ifndef H_SEARCH
#define H_SEARCH


#include <stdint.h>
#include <stdlib.h>

#include "cursor.h"
#include "msg.h"
#include "state.h"
#include "util.h"


struct h_query_t **h_search_parse(char *query, size_t *qsz);
void h_search_free(struct h_query_t **queries, size_t qsz);
void h_search_apply(struct h_state_t *state, char *query);
void h_search_reset(struct h_state_t *state);
void h_search_next(struct h_state_t *state);
void h_search_prev(struct h_state_t *state);

#endif
