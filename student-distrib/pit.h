#ifndef _PIT_H
#define _PIT_H

#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"

/* Irq that the PIT sits on */
#define PIT_IRQ 			0

// I/O port Usage of PIT device(IRQ 0)- 0x40, Channel 0 data port (read/write)
#define PIT_PORT			0x40

// Command register of the PIT
#define COM_REG_PORT		0x43

// command byte for square wave
#define SQUARE_WAVE 		0x36

// default PIT freq in Hz(not required to set by ourself)
#define DEFAULT_PIT_FREQ 	18

// Functions

/* Function to init pit*/
void pit_init(void);

/* Function to handle an pit interrupt*/
void pit_handler(void);

int32_t pit_open(const uint8_t* filename);
int32_t pit_read(int32_t fd, void* buf, int32_t nbytes);
int32_t pit_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t pit_close(int32_t fd);

// helper functions
void pit_timer_phase(int freq);

#endif	/* _PIT_H*/
