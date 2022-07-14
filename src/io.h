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

void h_cls(void);
void h_goto(uint16_t x, uint16_t y);
uint32_t h_ttysz(void);
void h_tty_setup(void);
void h_tty_restore(void);
int h_esc_lbracket_1(void);
int h_esc_lbracket(void);
int h_esc_o(void);
int h_esc(void);
int h_getkey(void);

#endif
