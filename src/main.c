#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "buffer.h"
#include "io.h"
#include "keys.h"
#include "render.h"
#include "state.h"

static struct h_state_t state = {0};

void sighandler(int signum) {
  switch (signum) {
    case SIGWINCH:
      h_tty_update(&state);
      break;

    case SIGINT:
      exit(0);
      break;

    default:
      break;
  }
}

void onexit() {
  h_tty_restore();
}

void h_show_help() {
  puts("hexane - HEX editor");
  puts("");
  puts("usage:");
  puts("    a       -- show ASCII representation");
  puts("    A       -- autosize viewport on TTY size change");
  puts("    c COLS  -- set number of columns to COLS");
  puts("    f FNAME -- open binary file named FNAME");
  puts("    h       -- show this help message and exit");
  puts("    H       -- show a header with column numbers");
  puts("    l LINES -- set the limit of the displayed rows");
  puts("");

  exit(0);
}

int main(int argc, char *argv[]) {
  atexit(onexit);
  signal(SIGWINCH, sighandler);
  signal(SIGINT, sighandler);

  char opt;
  const char *filename = NULL;

  h_tty_setup();
  h_state_init(&state);

  while ((opt = getopt(argc, argv, "aAc:f:hHl:")) != -1) {
    switch (opt) {
      case 'a':
        state.ascii = true;
        break;

      case 'A':
        state.autosize = true;
        break;

      case 'c':
        state.cols = atoi(optarg);
        break;

      case 'f':
        filename = optarg;
        break;

      case 'h':
        h_show_help();
        break;

      case 'H':
        state.header = true;
        break;

      case 'l':
        state.lines = atoi(optarg);
        break;
    }
  }

  if (filename != NULL) {
    h_edit_file(&state, filename);
  }

  h_render(&state);

  for (;;) {
    int key = h_getkey();

    if (key > 0) {
      h_key_handle(key, &state);
    }
  }

  h_tty_restore();

  return 0;
}
