#include "search.h"


struct h_query_t **h_search_parse(char *query, size_t *qsz) {
  size_t queries_sz = 0;
  char **queries = h_strsplit(query, ",", &queries_sz);

  struct h_query_t **qs = NULL;
  *qsz = 0;

  for (int i = 0; i < queries_sz; i++) {
    char *query = queries[i];

    (*qsz)++;
    qs = realloc(qs, *qsz * sizeof (struct h_query_t *));

    struct h_query_t *q = malloc(sizeof (struct h_query_t));
    q->terms = NULL;
    q->terms_sz = 0;
    qs[*qsz-1] = q;

    size_t qterms_sz = 0;
    char **qterms = h_strsplit(query, " ", &qterms_sz);

    for (int j = 0; j < qterms_sz; j++) {
      uint8_t term = strtoul(qterms[j], NULL, 0);
      q->terms_sz++;
      q->terms = realloc(q->terms, sizeof (uint8_t) * q->terms_sz);
      q->terms[q->terms_sz-1] = term;
    }

    h_strsplit_free(qterms, qterms_sz);
  }

  h_strsplit_free(queries, queries_sz);

  return qs;
}

void h_search_free(struct h_query_t **queries, size_t qsz) {
  for (int i = 0; i < qsz; i++) {
    free(queries[i]->terms);
    free(queries[i]);
  }
  free(queries);
}

bool h_query_match(struct h_query_t *q, uint8_t *buf) {
  for (int i = 0; i < q->terms_sz; i++) {
    if (buf[i] != q->terms[i]) {
      return false;
    }
  }
  return true;
}

void h_search_apply(struct h_state_t *state, char *query) {
  h_search_reset(state);

  size_t queries_sz = 0;
  struct h_query_t **queries = h_search_parse(query, &queries_sz);
  size_t found = 0;


  for (int i = 0; i < state->bufsz; i++) {
    for (int j = 0; j < queries_sz; j++) {
      if (h_query_match(queries[j], &state->buffer[i])) {
        found++;
        state->searchposbuf[i] = 1;
        for (int k = 0; k < queries[j]->terms_sz; k++) {
          state->searchbuf[i+k] = 1;
        }
      }
    }
  }

  h_search_free(queries, queries_sz);

  state->search_result_count = found;

  h_msg(state, "Found %zu item(s).", found);
}

void h_search_reset(struct h_state_t *state) {
  memset(state->searchposbuf, 0, state->bufsz);
  memset(state->searchbuf, 0, state->bufsz);
  state->search_result_count = 0;
}

void h_search_next(struct h_state_t *state) {
  if (!state->search_result_count) {
    return;
  }

  while (!state->searchposbuf[++state->searchpos]) {
    if (state->searchpos >= state->bufsz) {
      state->searchpos = 0;
    }
  }
  h_cursor_goto(state, state->searchpos);
}

void h_search_prev(struct h_state_t *state) {
  if (!state->search_result_count) {
    return;
  }

  while (!state->searchposbuf[--state->searchpos]) {
    if (state->searchpos >= state->bufsz) {
      state->searchpos = 0;
    }
  }
  h_cursor_goto(state, state->searchpos);
}
