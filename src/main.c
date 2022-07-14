#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "io.h"
#include "hex.h"


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

int main(int argc, char *argv[]) {
  atexit(onexit);
  signal(SIGWINCH, sighandler);
  signal(SIGINT, sighandler);

  char opt;
  const char *filename = NULL;

  while ((opt = getopt(argc, argv, "f:")) != -1) {
    filename = optarg;
  }

  h_tty_setup();
  h_state_init(&state);

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
