#include "io.h"


static struct termios encountered_term;

struct pollfd fds = {
  .fd = STDIN_FILENO,
  .events = POLLIN
};

// Clears the screen
void h_tty_cls() {
  printf("\x1b[2J");
}

// Moves the terminal cursor to position (x, y).
void h_tty_goto(uint16_t x, uint16_t y) {
  printf("\x1b[%d;%dH", y, x);
}

// Returns the TTY size combined into a single 32-bit value.
// The higher 16 bits represent the width and the lower ones represent the
// height.
uint32_t h_ttysz() {
  struct winsize wsz;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &wsz);
  return (wsz.ws_col << 16) | (wsz.ws_row & 0xffff);
}

// Sets the TTY attributes for the program.
void h_tty_setup(void) {
  tcgetattr(STDIN_FILENO, &encountered_term);

  struct termios new_term;
  memcpy(&new_term, &encountered_term, sizeof (struct termios));

  new_term.c_lflag &= ~ICANON;
  new_term.c_lflag &= ~ECHO;

  tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
}

// Restores the TTY attributes encountered at the start of the program.
void h_tty_restore(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &encountered_term);
}

// Handles the escape sequence keys
int h_esc_lbracket_1() {
  int pollres = poll(&fds, 1, 50);

  if (pollres < 0) {
    return pollres;
  }

  if (pollres == 0) {
    return 27;
  }

  char c;
  read(STDIN_FILENO, &c, 1);

  switch (c) {
    case '5': {
      read(STDIN_FILENO, &c, 1);
      return H_K_F5;
    }
    default: return 27;
  }
}

int h_esc_lbracket() {
  int pollres = poll(&fds, 1, 50);

  if (pollres < 0) {
    return pollres;
  }

  if (pollres == 0) {
    return 27;
  }

  char c;
  read(STDIN_FILENO, &c, 1);

  switch (c) {
    case 'A': return H_K_UP;
    case 'B': return H_K_DOWN;
    case 'C': return H_K_RIGHT;
    case 'D': return H_K_LEFT;
    case '1': return h_esc_lbracket_1();
    default: return 27;
  }
}

int h_esc_o() {
  int pollres = poll(&fds, 1, 50);

  if (pollres < 0) {
    return pollres;
  }

  if (pollres == 0) {
    return 27;
  }

  char c;
  read(STDIN_FILENO, &c, 1);

  switch (c) {
    case 'P': return H_K_F1;
    case 'Q': return H_K_F2;
    case 'R': return H_K_F3;
    case 'S': return H_K_F4;
    default: return 27;
  }
}

int h_esc() {
  int pollres = poll(&fds, 1, 50);

  if (pollres < 0) {
    return pollres;
  }

  if (pollres == 0) {
    return 27;
  }

  char c;
  read(STDIN_FILENO, &c, 1);

  switch (c) {
    case '[': return h_esc_lbracket();
    case 'O': return h_esc_o();
    default: return 27;
  }
}

int h_getkey() {

  int pollres = poll(&fds, 1, 100);

  if (pollres < 1) {
    return pollres;
  }

  char c;
  read(STDIN_FILENO, &c, 1);

  if (c != 27) {
    return c;
  }

  // ESC Sequence
  return h_esc();
}
