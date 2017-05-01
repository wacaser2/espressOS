
#ifndef _MOUSE_H
#define _MOUSE_H

#include "lib.h"
#include "i8259.h"
#include "rtc.h"


void mouse_init(void);

void mouse_handler(void);

int32_t mouse_open(void);

uint8_t mouse_read(void);

void mouse_write(uint8_t a_write);

int32_t mouse_close(void);

//void mouse_wait(unsigned char type);

#endif
