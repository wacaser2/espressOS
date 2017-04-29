#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "x86_desc.h"
#include "lib.h"

/* Ports that Keyboard sits on */
#define KEYBOARD_PORT 0x60

/* Irq that the Keyboard sits on */
#define KEYBOARD_IRQ 1

/* keyboard buffer size */
#define KEY_BUF_SIZE 130
#define KEY_BUF_SIZE_ACTUAL 128

#define BACKSPACE 	0x0E
#define ENTER 		0x1C
#define F_START			0x3b

#define LEFT_CTRL	0x1D
#define LEFT_SHIFT	0x2A
#define RIGHT_SHIFT	0x36
#define CAPS_LOCK	0x3A
#define ALT			0x38

#define LEFT_CTRL_REL	0x9D
#define LEFT_SHIFT_REL	0xAA
#define RIGHT_SHIFT_REL	0xB6
#define CAPS_LOCK_REL	0xBA
#define ALT_REL			0xB8

#define NULL_KEY	'\0'
#define NEW_LINE	'\n'
#define CARRIAGE_RETURN '\r'

#define LEFT_ARROW		75
#define RIGHT_ARROW		77
#define UP_ARROW		72
#define DOWN_ARROW		80


/* Initialize both Keyboard */
void keyboard_init();

/* handles keyboard input by echoing char pressed to the screen */
void keyboard_handler();

int32_t terminal_open(const uint8_t* filename);
int32_t terminal_read(int32_t fd, void * buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void * buf, int32_t nbytes);
int32_t terminal_close(int32_t fd);

#endif	/* _KEYBOARD_H*/
