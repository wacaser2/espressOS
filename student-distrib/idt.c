/*File for all of the functions for idt*/

#include "x86_desc.h"
#include "lib.h"
#include "syscalls.h"

/* Array of exception messages */
char* exception_messages[32] = {
    "Division By Zero",
    "RESERVED by Intel",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode (undefined)",
    "Device Not Available (Math Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun (reserved)",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection",
    "Page Fault",
    "(Intel reserved. Do not use.)",
    "x87 FPU Floating-Point Error (Math Fault)",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};


/* Function to print the exception messages *//*
 * fault_handler
 *   DESCRIPTION: Handles each of the exceptions by printing the error and the error code
 *   INPUTS: s - the state of the stack when this function is called
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Will set the screen color blue and enter an infinite loop
 */
void fault_handler(isr_stack_t *s){
	if(s->int_no < 32){
		printf("%s : %d\n", exception_messages[s->int_no], s->err_code);		//print error
		setlinecolor(0x14);
		//setcolor(0x17);	//make screen blue
		halt(255);
	}
}


/* Function to install all of the Interrupt Service Routines *//*
 * isrs_install()
 *   DESCRIPTION: Sets up the idt entries which we have handlers for
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets up the IDT
 */
void isrs_install(){
	SET_IDT_ENTRY(idt[0], (unsigned)isr0, 0);
	SET_IDT_ENTRY(idt[1], (unsigned)isr1, 0);
	SET_IDT_ENTRY(idt[2], (unsigned)isr2, 0);
	SET_IDT_ENTRY(idt[3], (unsigned)isr3, 0);
	SET_IDT_ENTRY(idt[4], (unsigned)isr4, 0);
	SET_IDT_ENTRY(idt[5], (unsigned)isr5, 0);
	SET_IDT_ENTRY(idt[6], (unsigned)isr6, 0);
	SET_IDT_ENTRY(idt[7], (unsigned)isr7, 0);
	SET_IDT_ENTRY(idt[8], (unsigned)isr8, 0);
	SET_IDT_ENTRY(idt[9], (unsigned)isr9, 0);
	SET_IDT_ENTRY(idt[10], (unsigned)isr10, 0);
	SET_IDT_ENTRY(idt[11], (unsigned)isr11, 0);
	SET_IDT_ENTRY(idt[12], (unsigned)isr12, 0);
	SET_IDT_ENTRY(idt[13], (unsigned)isr13, 0);
	SET_IDT_ENTRY(idt[14], (unsigned)isr14, 0);
	SET_IDT_ENTRY(idt[15], (unsigned)isr15, 0);
	SET_IDT_ENTRY(idt[16], (unsigned)isr16, 0);
	SET_IDT_ENTRY(idt[17], (unsigned)isr17, 0);
	SET_IDT_ENTRY(idt[18], (unsigned)isr18, 0);
	SET_IDT_ENTRY(idt[19], (unsigned)isr19, 0);
	SET_IDT_ENTRY(idt[20], (unsigned)isr20, 0);
	SET_IDT_ENTRY(idt[21], (unsigned)isr21, 0);
	SET_IDT_ENTRY(idt[22], (unsigned)isr22, 0);
	SET_IDT_ENTRY(idt[23], (unsigned)isr23, 0);
	SET_IDT_ENTRY(idt[24], (unsigned)isr24, 0);
	SET_IDT_ENTRY(idt[25], (unsigned)isr25, 0);
	SET_IDT_ENTRY(idt[26], (unsigned)isr26, 0);
	SET_IDT_ENTRY(idt[27], (unsigned)isr27, 0);
	SET_IDT_ENTRY(idt[28], (unsigned)isr28, 0);
	SET_IDT_ENTRY(idt[29], (unsigned)isr29, 0);
	SET_IDT_ENTRY(idt[30], (unsigned)isr30, 0);
	SET_IDT_ENTRY(idt[31], (unsigned)isr31, 0);
	SET_IDT_ENTRY(idt[0x20], (unsigned)pit_handler_wrapper, 0);
	SET_IDT_ENTRY(idt[0x21], (unsigned)keyboard_handler_wrapper, 0);
	SET_IDT_ENTRY(idt[0x28], (unsigned)rtc_handler_wrapper, 0);
	SET_IDT_ENTRY(idt[0x2C], (unsigned)mouse_handler_wrapper, 0);
	SET_IDT_ENTRY(idt[0x80], (unsigned)sys_call, 3);
}
