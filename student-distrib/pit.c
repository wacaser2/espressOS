#include "pit.h"

volatile int timer_ticks = 0;
volatile int current_freq = 18;

/* Function to init pit*/
void 
pit_init(void)
{
	enable_irq(PIT_IRQ);
}

/* Function to handle an pit interrupt */
void 
pit_handler(void)
{
	send_eoi(PIT_IRQ);

	 /* Increment our 'tick count' */
    timer_ticks++;

    /* Every current_freq clocks (approximately 1 second becasue default freq is current_freq Hz), we will
    *  display a message on the screen */
    // if (timer_ticks % current_freq == 0)
    //     puts("One second has passed\n");

}

int32_t 
pit_open(const uint8_t* filename)
{
	pit_timer_phase(DEFAULT_PIT_FREQ);	// sets PIT freq to 18 Hz
	return 0;
}

int32_t 
pit_read(int32_t fd, void* buf, int32_t nbytes)
{
	// int rate = inb(PIT_PORT);
	// int freq = (rate & 0xFF) | (rate >> 8);
	// return freq;
	return current_freq;
}

int32_t 
pit_write(int32_t fd, const void* buf, int32_t nbytes)
{
	if(buf == NULL || nbytes != 4)
		return -1;

	pit_timer_phase(*(int32_t*)buf);	// set PIT to frequency value in buf	

	return 0;
}

int32_t 
pit_close(int32_t fd)
{
	return 0;
}

void pit_timer_phase(int freq)
{
	cli();

	current_freq = freq;
    int freq_divisor = 1193180 / freq;					/* Calculate our divisor */
    outb(SQUARE_WAVE, COM_REG_PORT);             		/* Set our command byte 0x36 */
    outb(freq_divisor & 0xFF, PIT_PORT);   				/* Set low byte of divisor */
    outb(freq_divisor >> 8, PIT_PORT);    				/* Set high byte of divisor */

	sti();
}
