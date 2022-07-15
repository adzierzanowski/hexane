#include "cmdline.h"


// Resets the command line buffer and exits the command line mode.
void h_cmd_clear(struct h_state_t *state) {
  memset(state->cmdbuf, 0, H_CMDBUFSZ);
  state->cmdpos = 0;
  state->cmdline = false;
}

// Handles the command passed by the user in the command line mode.
void h_cmd_handle(struct h_state_t *state) {
  char *cmd = state->cmdbuf;

  if (!cmd[0]) {
    h_msg(state, "Empty command.");
    h_cmd_clear(state);
    return;
  }

  if (strcmp(cmd, "q") == 0) {
    exit(0);

  } else if (h_strmatch(cmd, "a", "ascii", NULL)) {
    state->ascii = !state->ascii;
    h_msg(state, "Show ASCII set to %s", state->ascii ? "true" : "false");

  } else if (h_strmatch(cmd, "h", "header", NULL)) {
    state->header = !state->header;
    h_msg(state, "Show header set to %s.", state->header ? "true" : "false");

  } else if (h_strmatch(cmd, "as", "autosize", NULL)) {
    state->autosize = !state->autosize;
    h_msg(state, "Autosize set to %s.", state->autosize ? "true" : "false");

  } else {
    char *tok = strtok(state->cmdbuf, " ");

    if (h_strmatch(tok, "c", "cols", NULL)) {
      tok = strtok(NULL, " ");

      if (tok == NULL) {
        h_msg(state, "Column number: %d", state->cols);
        h_cmd_clear(state);
        return;
      }

      int n = atoi(tok);
      if (n > 0 && n <= 128) {
        state->cols = n;
        h_msg(state, "Column number changed to %d", n);
      } else {
        h_msg(state, "Bad argument for cols.");
      }

    } else if (h_strmatch(tok, "e", "edit", NULL)) {
      tok = strtok(NULL, " ");
      if (tok == NULL) {
        h_msg(state, "No filename provided.");
        h_cmd_clear(state);
        return;
      }

      h_edit_file(state, tok);

    } else if (h_strmatch(tok, "w", "write", NULL)) {
      tok = strtok(NULL, " ");
      if (tok == NULL) {
        if (state->fname[0] != 0) {
          h_save_file(state, tok);
        } else {
          h_msg(state, "No filename provided.");
        }

      } else {
        h_save_file(state, tok);
      }

    } else if (h_strmatch(tok, "l", "lines", NULL)) {
      tok = strtok(NULL, " ");

      if (tok == NULL) {
        h_msg(state, "Line limit: %d", state->lines);
        h_cmd_clear(state);
        return;
      }

      int lines = atoi(tok);

      if (lines > 0 && lines < state->tch) {
        state->lines = lines;
        h_tty_update(state);
        h_msg(state, "Set line limit to %d", state->lines);

      } else {
        h_msg(state, "Bad argument for line limit.");
      }

    } else {
      h_msg(state, "Unknown command: %s", state->cmdbuf);
    }
  }

  h_cmd_clear(state);
}
