
#include "rtc.h"
#include "lib.h"
#include "x86_desc.h"

void
rtc_init(void)
{
	/*CLI();		// important that no interrupts happen (perform a CLI)
	outb(STATUS_REGISTER_A, RTC_PORT);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
	outb(0x20, CMOS_PORT);	// write to CMOS/RTC RAM
	STI();		// (perform an STI) and reenable NMI if you wish*/

	cli();			// disable interrupts
	outb(STATUS_REGISTER_B, RTC_PORT);		// select register B, and disable NMI
	char prev = inb(CMOS_PORT);	// read the current value of register B
	outb(STATUS_REGISTER_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
	outb(prev | 0x40, CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
	sti();
}


void
rtc_handler(void)
{
	outb(STATUS_REGISTER_C, RTC_PORT);
	inb(CMOS_PORT);
	puts("a");
	// test_interrupts();
}
