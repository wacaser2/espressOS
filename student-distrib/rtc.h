#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "lib.h"
// #include "keyboard.h"
#include "x86_desc.h"
/* Irq that the RTC sits on */
#define RTC_IRQ 8

/* Port for rtc */
#define RTC_PORT			0x70
/* Port for the cmos*/
#define CMOS_PORT			0x71
/* Status registers*/
#define STATUS_REGISTER_A	0x8A
#define STATUS_REGISTER_B	0x8B
#define STATUS_REGISTER_C	0x0C
/*mask to insert rate into lower 4 bits of register A*/
#define RATE_MASK			0xF0
#define DEFAULT_FREQ		2

#define FREQ_LIMIT			1024
#define RATE_at_FREQ_LIMIT	0x06

/* Function to init rtc*/
void rtc_init(void);

/* Function to handle an rtc interrupt*/
void rtc_handler(void);

int32_t rtc_open(const uint8_t* filename);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_close(int32_t fd);
int32_t set_freq(int32_t freq);

#endif
