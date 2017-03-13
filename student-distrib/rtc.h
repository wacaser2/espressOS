
#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "lib.h"
// #include "keyboard.h"
#include "x86_desc.h"
/* Irq that the Keyboard sits on */
#define RTC_IRQ 8

/* Port for rtc */
#define RTC_PORT			0x70
/* Port for the cmos*/
#define CMOS_PORT			0x71
/* Status registers*/
#define STATUS_REGISTER_A	0x8A
#define STATUS_REGISTER_B	0x8B
#define STATUS_REGISTER_C	0x0C

/* Function to init rtc*/
void rtc_init(void);

/* Function to handle an rtc interrupt*/
void rtc_handler(void);

#endif
