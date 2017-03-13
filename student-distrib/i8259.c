/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
#include "i8259.h"
#include "lib.h"

 /* Interrupt masks to determine which interrupts
  * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

//static spinlock_t i8259_lock = SPIN_LOCK_UNLOCKED;

/* Initialize the 8259 PIC *//*
 * i8259_init()
 *   DESCRIPTION: Initializes the PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets up the PIC and set the masks to full
 */
void
i8259_init(void)
{
	/* init master*/
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW2_MASTER, MASTER_8259_PORT2);
	outb(ICW3_MASTER, MASTER_8259_PORT2);
	outb(ICW4, MASTER_8259_PORT2);
	/* init slave*/
	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_SLAVE, SLAVE_8259_PORT2);
	outb(ICW3_SLAVE, SLAVE_8259_PORT2);
	outb(ICW4, SLAVE_8259_PORT2);
	/* set mask to not let any interrupts through*/
	outb(FULL_MASK, MASTER_8259_PORT2);
	outb(FULL_MASK, SLAVE_8259_PORT2);
	/* set our local masks to full*/
	master_mask = slave_mask = FULL_MASK;
	/* enable the slave's interrupt line on master*/
	enable_irq(ICW3_SLAVE);
}

/* Enable (unmask) the specified IRQ *//*
 * enable_irq
 *   DESCRIPTION: Unmasks the specified interrupt request line
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Allows interrupts from the requested line to occur
 */
void
enable_irq(uint32_t irq_num)
{
	if (irq_num >= MASTER_START && irq_num < PIC_SIZE + MASTER_START)	// master IRQ
	{
		/* update our master mask*/
		master_mask &= ~(1 << (irq_num));
		/* send new mask to master*/
		outb(master_mask, MASTER_8259_PORT2);
	}
	else if (irq_num >= SLAVE_START && irq_num < SLAVE_START + PIC_SIZE)  // slave IRQ
	{
		/* update our slave mask*/
		slave_mask &= ~(1 << (irq_num - SLAVE_START));
		/* send the new slave mask to the slave*/
		outb(slave_mask, SLAVE_8259_PORT2);
	}
}

/* Disable (mask) the specified IRQ *//*
 * disable_irq
 *   DESCRIPTION: Masks the specified interrupt request line
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Disables interrupts from the requested line
 */
void
disable_irq(uint32_t irq_num)
{
	if (irq_num >= MASTER_START && irq_num < PIC_SIZE + MASTER_START)	// master IRQ
	{
		/* update our master mask*/
		master_mask |= (1 << irq_num);
		/* send the new master mask to the master*/
		outb(master_mask, MASTER_8259_PORT2);
	}
	else if (irq_num >= SLAVE_START && irq_num < SLAVE_START + PIC_SIZE)  // slave IRQ
	{
		/* update our slave mask*/
		slave_mask |= (1 << (irq_num - SLAVE_START));
		/* send the new slave mask to the slave*/
		outb(slave_mask, SLAVE_8259_PORT2);
	}
}

/* Send end-of-interrupt signal for the specified IRQ *//*
 * send_eoi
 *   DESCRIPTION: Tells the PIC that you have received the interrupt from the specified line
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void
send_eoi(uint32_t irq_num)
{
	if (irq_num >= MASTER_START && irq_num < PIC_SIZE + MASTER_START)	// master IRQ
	{
		/* inform master of end of interrupt*/
		outb((EOI | irq_num), MASTER_8259_PORT);
	}
	else if (irq_num >= SLAVE_START && irq_num < SLAVE_START + PIC_SIZE)  // slave IRQ
	{
		/* inform master and slave of end of interrupt*/
		outb((EOI | (irq_num - SLAVE_START)), SLAVE_8259_PORT);
		outb((EOI | TWO_SLAVE), MASTER_8259_PORT);
	}
}
