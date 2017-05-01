#ifndef _BOOTUP_H
#define _BOOTUP_H

#include "pit.h"
#include "lib.h"
#include "keyboard.h"
#include "types.h"

#define BACKGROUND 		0x77
#define TEXT 			0x76

/* LOGIN CREDENTIALS*/
#define USER_NAME		"espressos"
#define PASSWORD		"memes"

#define FAILURE				-1
#define SUCCESS				0

void bootup_sequence_1();
void login_screen();

#endif	/* _BOOTUP_H*/
