#include "keys.h"


// Handles the key pressed by the user in the command line mode.
void h_key_handle_cmd(int key, struct h_state_t *state) {
  switch (key) {
    case '\x1b': {
      h_cmd_clear(state);
      break;
    }

    case '\x7f': {
      if (state->cmdpos > 0) {
        state->cmdbuf[--state->cmdpos] = 0;
      }
      break;
    }

    case '\n': {
      h_cmd_handle(state);
      break;
    }

    default: {
      if (state->cmdpos < H_CMDBUFSZ && (isprint(key))) {
        state->cmdbuf[state->cmdpos++] = (char) key;
      }

      break;
    }
  }

  h_render(state);
}

void h_key_handle_color(int key, struct h_state_t *state) {
  switch (key) {
    case '\x1b':
      state->color_picker = false;
      state->color_pos = state->color;
      h_render(state);
      return;

    case '\n':
      state->color_picker = false;
      state->color = state->color_pos;
      h_render(state);
      return;

    case 'h':
      state->color_pos -= 1;
      break;

    case 'j':
      state->color_pos += 16;
      break;

    case 'k':
      state->color_pos -= 16;
      break;

    case 'l':
      state->color_pos += 1;
      break;
  }

  state->color_pos %= 0x100;

  h_render_color_picker(state);
}

// Handles the key pressed by the user in the normal mode.
void h_key_handle(int key, struct h_state_t *state) {
  state->last_key = key;

  if (state->color_picker) {
    h_key_handle_color(key, state);
    return;
  }

  if (state->cmdline) {
    h_key_handle_cmd(key, state);
    return;
  }

  switch (key) {
    case '0':
      h_cursor_move(state, -state->cursor_pos % state->cols);
      break;

    case '$':
      h_cursor_move(state, state->cols - (state->cursor_pos % state->cols) - 1);
      break;

    case 'g':
      h_cursor_goto(state, 0);
       break;

    case 'G':
      h_cursor_goto(state, state->bufsz-1);
      break;

    case 'b':
      h_cursor_move(state, -state->cols / 8);
      break;

    case 'w':
      h_cursor_move(state, state->cols / 8);
      break;

    case 'h':
      h_cursor_move(state, -1);
      break;

    case 'j':
      h_cursor_move(state, state->cols);
      break;

    case 'k':
      h_cursor_move(state, -state->cols);
      break;

    case 'l':
      h_cursor_move(state, 1);
      break;

    case '[':
      h_cursor_move(state, -state->lines * state->cols / 2);
      break;

    case ']':
      h_cursor_move(state, state->lines * state->cols / 2);
      break;

    case ':':
      state->cmdline = true;
      break;

    case '/':
      state->cmdline = true;
      sprintf(state->cmdbuf, "/ ");
      state->cmdpos = 2;
      break;

    case 'n':
      h_search_next(state);
      break;

    case 'N':
      h_search_prev(state);
      break;

    case 'v':
      h_select(state);
      break;

    case '=':
      h_buffer_increment(state);
      break;

    case '-':
      h_buffer_decrement(state);
      break;

    case '\x1b':
      h_selection_clear(state);
      h_search_reset(state);
      break;

    case H_K_F2:
      state->color_picker = true;
      break;

    case 'P':
      h_color_eyedrop(state);
      break;

    case 'm':
      h_mark_bytes(state);
      break;

    case 'x':
      state->hex = !state->hex;
      h_msg(
        state,
        "Number mode changed to %s.", state->hex ? "hexadecimal" : "decimal");
      break;

    default:
      break;
  }

  h_selection_update(state);
  h_render(state);
}
