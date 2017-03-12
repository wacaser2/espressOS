/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
#include "i8259.h"
#include "lib.h"

#define FULL_MASK 		0xFF
#define FULL_UNMASK		0x00
#define TWO_SLAVE		0x02

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

//static spinlock_t i8259_lock = SPIN_LOCK_UNLOCKED;

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
	// master_mask = inb(MASTER_8259_PORT2);
	// slave_mask = inb(SLAVE_8259_PORT2);

	outb(FULL_MASK, MASTER_8259_PORT2);
	outb(FULL_MASK, SLAVE_8259_PORT2);

	outb(ICW1, MASTER_8259_PORT);
	outb(ICW2_MASTER, MASTER_8259_PORT2);
	outb(ICW3_MASTER, MASTER_8259_PORT2);
	outb(ICW4, MASTER_8259_PORT2);

	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_SLAVE, SLAVE_8259_PORT2);
	outb(ICW3_SLAVE, SLAVE_8259_PORT2);
	outb(ICW4, SLAVE_8259_PORT2;

	// outb(master_mask, MASTER_8259_PORT + 1);
	// outb(slave_mask, SLAVE_8259_PORT + 1);

	outb(FULL_UNMASK, MASTER_8259_PORT2);
	outb(FULL_UNMASK, SLAVE_8259_PORT2);
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	uint8_t enable_mask = 0xFE;
	if(irq_num >= 0 && irq_num <= 7)	// master IRQ
	{
		uint32_t i;
		for(i = 0; i < irq_num; i++)
		{
			enable_mask <<= 1;
			enable_mask += 1;
		}
		master_mask &= enable_mask;
		outb(master_mask, MASTER_8259_PORT2);
	}
	else if(irq_num >= 8 && irq_num <= 15)  // slave IRQ
	{
		uint32_t i;
		for(i = 0; i < (irq_num - 8); i++)
		{
			enable_mask <<= 1;
			enable_mask += 1;
		}
		slave_mask &= enable_mask;
		outb(slave_mask, SLAVE_8259_PORT2);
	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	uint8_t disable_mask = 0x01;
	if(irq_num >= 0 && irq_num <= 7)	// master IRQ
	{
		uint32_t i;
		for(i = 0; i < irq_num; i++)
		{
			disable_mask <<= 1;
		}
		master_mask |= disable_mask;
		outb(master_mask, MASTER_8259_PORT2);
	}
	else if(irq_num >= 8 && irq_num <= 15)  // slave IRQ
	{
		uint32_t i;
		for(i = 0; i < (irq_num - 8); i++)
		{
			disable_mask <<= 1;
		}
		slave_mask |= disable_mask;
		outb(slave_mask, SLAVE_8259_PORT2);
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= 0 && irq_num <= 7)	// master IRQ
	{
		outb((EOI | irq_num), MASTER_8259_PORT);
	}
	else if(irq_num >= 8 && irq_num <= 15)  // slave IRQ
	{
		outb((EOI | (irq_num - 8)), SLAVE_8259_PORT);
		outb((EOI | TWO_SLAVE), MASTER_8259_PORT);
	}
}  


