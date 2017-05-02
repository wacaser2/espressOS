
#ifndef _MOUSE_H
#define _MOUSE_H

#include "lib.h"
#include "i8259.h"
#include "rtc.h"

/* Ports that Mouse sends data to */
#define MOUSE_PORT	 		0x60

/* Port that tells us if there is anything to read from Mouse port*/ 
#define MOUSE_PORT_INFO		0x64

/* IRQ line that the mouse sits on */
#define MOUSE_IRQ			12

/* Command to reset*/ 
#define MOUSE_RESET			0xFF

/* Command to set defaults, NOTE: Mouse responds with an acknowledge(MOUSE_ACK)*/ 
#define SET_DEFAUTLS		0xF6

/* Mouse sends this after it acknowledges */ 
#define MOUSE_ACK			0xFA

/* Command to set sample rate */ 
#define SET_SR				0xF3

/* Command to enable data reporting*/ 
#define ENABLE_DR			0xF4

/* Helps us tell the mosue that we are sending it a command*/
#define XD4_BYTE			0xD4

/* Command to get mouse id*/
#define GET_MOUSE_ID		0xF2

/* Byte to enable Auxiliary mouse device*/
#define ENALBE_AUX			0xA8

/* Byte to tell the mouse to send us the status byte */
#define GET_SB				0x20


// Functions
void mouse_init(void);
void mouse_handler(void);

int32_t mouse_open(void);
uint8_t mouse_read(void);
void mouse_write(uint8_t a_write);
int32_t mouse_close(void);

void mouse_wait(uint8_t a_type);

#endif
