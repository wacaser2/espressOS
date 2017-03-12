
#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "lib.h"
// #include "keyboard.h"
#include "x86_desc.h"

#define RTC_PORT			0x70
#define CMOS_PORT			0x71
#define STATUS_REGISTER_A	0x8A
#define STATUS_REGISTER_B	0x8B
#define STATUS_REGISTER_C	0x0C

void rtc_init(void);
void rtc_handler(void);

#endif
