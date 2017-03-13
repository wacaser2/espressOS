#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "x86_desc.h"

/* Ports that Keyboard sits on */
#define KEYBOARD_PORT 0x60

/* Irq that the Keyboard sits on */
#define KEYBOARD_IRQ 1

/* Initialize both Keyboard */
void keyboard_init();

/* handles keyboard input by echoing char pressed to the screen */
void keyboard_handler();

#endif	/* _KEYBOARD_H*/
