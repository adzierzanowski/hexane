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

  // No-argument commands
  if (strcmp(cmd, "q") == 0) {
    exit(0);

  } else if (h_strmatch(cmd, "v", "version", NULL)) {
    h_msg(
      state,
      "%s v%s  %s %s  %s",
      H_PROGNAME, H_VERSION, __DATE__, __TIME__, __VERSION__);

  } else if (h_strmatch(cmd, "a", "ascii", NULL)) {
    state->ascii = !state->ascii;
    h_msg(state, "Show ASCII set to %s", state->ascii ? "true" : "false");

  } else if (h_strmatch(cmd, "h", "header", NULL)) {
    state->header = !state->header;
    h_msg(state, "Show header set to %s.", state->header ? "true" : "false");

  } else if (h_strmatch(cmd, "as", "autosize", NULL)) {
    state->autosize = !state->autosize;
    h_msg(state, "Autosize set to %s.", state->autosize ? "true" : "false");

  // Commands taking arguments
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

    } else if (h_strmatch(cmd, "com", "comment", NULL)) {
      char *tok = strtok(NULL, "\n");

      if (!tok) {
        h_msg(state, "Comment requires a string argument.");
        h_cmd_clear(state);
        return;
      }

      h_comment_add(state, tok, state->cursor_pos);

    } else if (h_strmatch(cmd, "comdel", NULL)) {
      h_comment_remove(state, state->cursor_pos);

    } else if (h_strmatch(tok, "e", "edit", NULL)) {
      tok = strtok(NULL, " ");
      if (tok == NULL) {
        h_msg(state, "No filename provided.");
        h_cmd_clear(state);
        return;
      }

      h_edit_file(state, tok);
    } else if (h_strmatch(tok, "/", NULL)) {
      tok = strtok(NULL, "\n");
      if (!tok) {
        h_msg(state, "Search command needs at least one query.");
        h_cmd_clear(state);
        return;
      }

      h_search_apply(state, tok);

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

    } else if (h_strmatch(tok, "r", "replace", "sub", NULL)) {
      tok = strtok(NULL, " ");

      if (tok == NULL) {
        h_msg(state, "Replacing bytes requires a value.");
        h_cmd_clear(state);
        return;
      }

      char *endptr = NULL;
      int val = strtoul(tok, &endptr, 0);

      if (endptr && *endptr != 0) {
        h_msg(
          state,
          "%s is not a properly formatted value. "
          "E.g. for 20, use 20, 0x14 or 024.");
        h_cmd_clear(state);
        return;
      }

      h_buffer_set_bytes(state, val);
      size_t nbytes = state->selsize + state->cursel_size;
      h_msg(
        state,
        "Replaced %zu byte(s) with %u (0x%02x).",
        nbytes ? nbytes : 1,
        val,
        val);

    } else {
      h_msg(state, "Unknown command: %s", state->cmdbuf);
    }
  }

  h_cmd_clear(state);
}
