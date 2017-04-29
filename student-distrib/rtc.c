#include "i8259.h"
#include "rtc.h"
#include "lib.h"
#include "x86_desc.h"
#include "syscalls.h"

volatile int interrupt_flag = 0;

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
	send_eoi(RTC_IRQ);		//informs pic that we got the interrupt
	interrupt_flag = 1;
	if (get_proc() != -1) {
		pcb_t* block = get_pcb(get_proc());
		if (block->cycles == 0) {
			block->cycles = 0;
			int32_t next = get_proc_term();
			while (get_term_proc(next = (next + 1) % 3) == -1)
				;
			switch_process(next);
		}
		else {
			block->cycles--;
		}
	}
}


int32_t
rtc_open(const uint8_t* filename)
{
	set_freq(DEFAULT_FREQ); // set default frequency
	return 0; // return zero
}

int32_t
rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	interrupt_flag = 0;
	sti();
	while (interrupt_flag == 0) {} //  do nothing, keeping looping till an interrupt occurs
	cli();
	return 0;
}

int32_t
rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
	if (buf == NULL || nbytes != 4)
		return -1;
	int freq = *(int32_t*)buf;
	int ret = set_freq(freq); // ret gets return value of set_freq
	if (ret == -1) return ret; // if ret == -1 return -1
	return nbytes; // return nbytes value
}

int32_t
rtc_close(int32_t fd) //int32_t fd
{
	//if(fd <= 1) return -1;
	return 0;
}

int32_t
set_freq(int32_t freq)
{
	cli();

	outb(STATUS_REGISTER_A, RTC_PORT);		// set index to register A, disable NMI
	char prev = inb(CMOS_PORT);	// get initial value of register A

	unsigned char rate;			// interrupt rate

	switch (freq)			// all the rate values can be found on ds12887 datasheet
	{
	case FREQ_LIMIT:
		rate = RATE_at_FREQ_LIMIT;
		break;
	case FREQ_LIMIT >> 1:
		rate = RATE_at_FREQ_LIMIT + 1;
		break;
	case FREQ_LIMIT >> 2:
		rate = RATE_at_FREQ_LIMIT + 2;
		break;
	case FREQ_LIMIT >> 3:
		rate = RATE_at_FREQ_LIMIT + 3;
		break;
	case FREQ_LIMIT >> 4:
		rate = RATE_at_FREQ_LIMIT + 4;
		break;
	case FREQ_LIMIT >> 5:
		rate = RATE_at_FREQ_LIMIT + 5;
		break;
	case FREQ_LIMIT >> 6:
		rate = RATE_at_FREQ_LIMIT + 6;
		break;
	case FREQ_LIMIT >> 7:
		rate = RATE_at_FREQ_LIMIT + 7;
		break;
	case FREQ_LIMIT >> 8:
		rate = RATE_at_FREQ_LIMIT + 8;
		break;
	case FREQ_LIMIT >> 9:
		rate = RATE_at_FREQ_LIMIT + 9;
		break;
	default:
		return -1;
	}

	outb(STATUS_REGISTER_A, RTC_PORT);		// reset index to A
	outb((prev & RATE_MASK) | rate, CMOS_PORT); //write only our rate to A. Note, rate is the bottom 4 bits.

	sti();

	return 0;
}
