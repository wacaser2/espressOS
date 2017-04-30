#include "pit.h"

volatile int32_t timer_ticks = 0;
volatile int32_t current_freq = 18;
int32_t login = FAILURE;


/* Function to init pit*/
void
pit_init(void)
{
	enable_irq(PIT_IRQ);
	pit_timer_phase(100);
}

/* Function to handle an pit interrupt */
void
pit_handler(void)
{
	send_eoi(PIT_IRQ);
	cli();

	if(get_proc()==-1);
	else if(login==FAILURE)
	{
		login=2;
		login_screen();
	}
	else if (login == SUCCESS)
	{
		if (get_proc() != -1) {
			pcb_t* block = get_pcb(get_proc());
			if (block->cycles == 0) {
				block->cycles = 0;
				int32_t next, i;
				for (next = get_proc_term(), i = 1; i < 3; i++)
					if (get_term_proc((next + i) % 3) != -1) {
						switch_process((next + i) % 3);
						break;
					}
			}
			else {
				block->cycles--;
			}
		}
	}

	sti();
	/* Increment our 'tick count' */
   //timer_ticks++;

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
	if (buf == NULL || nbytes != 4)
		return -1;

	pit_timer_phase(*(int32_t*)buf);	// set PIT to frequency value in buf	

	return 0;
}

int32_t
pit_close(int32_t fd)
{
	return 0;
}

void pit_timer_phase(int32_t freq)
{
	cli();

	current_freq = freq;
	int32_t freq_divisor = 1193180 / freq;					/* Calculate our divisor */
	outb(SQUARE_WAVE, COM_REG_PORT);             		/* Set our command byte 0x36 */
	outb(freq_divisor & 0xFF, PIT_PORT);   				/* Set low byte of divisor */
	outb(freq_divisor >> 8, PIT_PORT);    				/* Set high byte of divisor */

	sti();
}


/* setter function to set login variable value */
void set_login_flag(int32_t login_status)
{
	login = login_status;
}
