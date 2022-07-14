#ifndef IO_H
#define IO_H

#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdint.h>

#include <sys/ioctl.h>

#define H_K_UP 0x101
#define H_K_DOWN 0x102
#define H_K_RIGHT 0x103
#define H_K_LEFT 0x104
#define H_K_F1 0x105
#define H_K_F2 0x106
#define H_K_F3 0x107
#define H_K_F4 0x108
#define H_K_F5 0x109
#define H_K_F6 0x10a
#define H_K_F7 0x10b
#define H_K_F8 0x10c

// TERMINAL ROUTINES

// Clears the terminal
void h_tty_cls(void);

// Moves the tty cursor to a position (x, y)
void h_tty_goto(uint16_t x, uint16_t y);

// Gets the current size of the terminal
// Returns the width in the higher word and the height in the lower word
uint32_t h_ttysz(void);

// Sets the mode of the TTY (non-canonical input, no echo, etc.)
void h_tty_setup(void);

// Restores the TTY state encountered on start of the program
void h_tty_restore(void);


// FETCHING INPUT ROUTINES

// Handles the ESC[1 escape sequences (function keys)
int h_esc_lbracket_1(void);

// Handles the ESC[ escape sequences
int h_esc_lbracket(void);

// Handles the ESCO escape sequences
int h_esc_o(void);

// Handles the escape sequences
int h_esc(void);

// Handles the user input on stdin
int h_getkey(void);


#endif
