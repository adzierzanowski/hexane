#include "state.h"


// Initializes the `state` with sensible defaults.
void h_state_init(struct h_state_t *state) {
  state->buffer = NULL;
  state->bufsz = 0;
  state->cursor_pos = 0;
  state->markbuf = NULL;
  state->combuf = NULL;

  state->offset = 0;

  uint32_t ttysz = h_ttysz();
  state->tcw = ttysz >> 16;
  state->tch = ttysz & 0xffff;
  state->lines = state->tch - H_STATUSLINESZ;

  state->cols = 2;
  while (state->cols * 3 < state->tcw) {
    state->cols *= 2;
  }
  state->cols /= 2;

  memset(state->cmdbuf, 0, H_CMDBUFSZ);
  state->cmdline = false;
  state->cmdpos = 0;

  memset(state->msgbuf, 0, H_MSGBUFSZ);
  state->ascii = false;
  state->header = false;
  state->autosize = false;
  state->hex = false;
  state->color_picker = false;
  state->color_pos = 51;
  state->color = 51;

  memset(state->fname, 0, H_BUFSZ);

  state->queries = NULL;
  state->searchbuf = NULL;
  state->searchposbuf = NULL;
  state->search_result_count = 0;
  state->searchpos = 0;

  state->selections = NULL;
  state->cursel = NULL;
  state->cursel_type = H_SELTYPE_NONE;
  state->selsize = 0;
  state->cursel_size = 0;
}

// Returns a number of the next byte to the last byte that should be
// displayed.
int h_getlim(struct h_state_t *state) {
  int lim = state->offset + state->cols * state->lines;
  lim = lim < state->bufsz ? lim : state->bufsz;
  return lim;
}
