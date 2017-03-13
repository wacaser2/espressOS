
#include "i8259.h"
#include "rtc.h"
#include "lib.h"
#include "x86_desc.h"

/*
* void rtc_init()
*   Inputs: none
*   Return Value: void
*	Function: Initializes the rtc interrupts
*/
void
rtc_init(void)
{
	outb(STATUS_REGISTER_B, RTC_PORT);		// select register B, and disable NMI
	char prev = inb(CMOS_PORT);	// read the current value of register B
	outb(STATUS_REGISTER_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
	outb(prev | 0x40, CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
	enable_irq(RTC_IRQ);				//enables the interrupts for rtc
}

/*
* void rtc_handler()
*   Inputs: none
*   Return Value: void
*	Function: Handles a rtc interrupt by running test_interrupts()
*/
void
rtc_handler(void)
{
	outb(STATUS_REGISTER_C, RTC_PORT);
	inb(CMOS_PORT);
	test_interrupts();
	send_eoi(RTC_IRQ);		//informs pic that we got the interrupt
	// test_interrupts();
}
