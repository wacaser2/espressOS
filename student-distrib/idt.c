/*File for all of the functions for idt*/

#include "x86_desc.h"

/* Array of exception messages */
const char* exception_messages[32] = {
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
//
//void idt_set_gate(unsigned char index, uint32_t offset, unsigned short seg, unsigned char flags){
//	long* idt_arr = (long*)idt;
//	idt_arr[(int32_t)index] = (((offset&0xFFFF0000)|(flags<<8))<<32)|((seg<<16)|(offset&0x0000FFFF));
//}

void fault_handler(isr_stack_t *s){
	if(s->int_no < 32){
		printf(exception_messages[s->int_no]);
		printf("\n");
		for(;;);
	}
}

void isrs_install(){
	SET_IDT_ENTRY(idt[0], (unsigned)isr0);
	SET_IDT_ENTRY(idt[1], (unsigned)isr0);
	SET_IDT_ENTRY(idt[2], (unsigned)isr0);
	SET_IDT_ENTRY(idt[3], (unsigned)isr0);
	SET_IDT_ENTRY(idt[4], (unsigned)isr0);
	SET_IDT_ENTRY(idt[5], (unsigned)isr0);
	SET_IDT_ENTRY(idt[6], (unsigned)isr0);
	SET_IDT_ENTRY(idt[7], (unsigned)isr0);
	SET_IDT_ENTRY(idt[8], (unsigned)isr0);
	SET_IDT_ENTRY(idt[9], (unsigned)isr0);
	SET_IDT_ENTRY(idt[10], (unsigned)isr0);
	SET_IDT_ENTRY(idt[11], (unsigned)isr0);
	SET_IDT_ENTRY(idt[12], (unsigned)isr0);
	SET_IDT_ENTRY(idt[13], (unsigned)isr0);
	SET_IDT_ENTRY(idt[14], (unsigned)isr0);
	SET_IDT_ENTRY(idt[15], (unsigned)isr0);
	SET_IDT_ENTRY(idt[16], (unsigned)isr0);
	SET_IDT_ENTRY(idt[17], (unsigned)isr0);
	SET_IDT_ENTRY(idt[18], (unsigned)isr0);
	SET_IDT_ENTRY(idt[19], (unsigned)isr0);
	SET_IDT_ENTRY(idt[20], (unsigned)isr0);
	SET_IDT_ENTRY(idt[21], (unsigned)isr0);
	SET_IDT_ENTRY(idt[22], (unsigned)isr0);
	SET_IDT_ENTRY(idt[23], (unsigned)isr0);
	SET_IDT_ENTRY(idt[24], (unsigned)isr0);
	SET_IDT_ENTRY(idt[25], (unsigned)isr0);
	SET_IDT_ENTRY(idt[26], (unsigned)isr0);
	SET_IDT_ENTRY(idt[27], (unsigned)isr0);
	SET_IDT_ENTRY(idt[28], (unsigned)isr0);
	SET_IDT_ENTRY(idt[29], (unsigned)isr0);
	SET_IDT_ENTRY(idt[30], (unsigned)isr0);
	SET_IDT_ENTRY(idt[31], (unsigned)isr0);

	/*
	idt_set_gate(0, (unsigned)isr0, IDT_SEG, ISR_FLAGS);
	idt_set_gate(1, (unsigned)isr1, IDT_SEG, ISR_FLAGS);
	idt_set_gate(2, (unsigned)isr2, IDT_SEG, ISR_FLAGS);
	idt_set_gate(3, (unsigned)isr3, IDT_SEG, ISR_FLAGS);
	idt_set_gate(4, (unsigned)isr4, IDT_SEG, ISR_FLAGS);
	idt_set_gate(5, (unsigned)isr5, IDT_SEG, ISR_FLAGS);
	idt_set_gate(6, (unsigned)isr6, IDT_SEG, ISR_FLAGS);
	idt_set_gate(7, (unsigned)isr7, IDT_SEG, ISR_FLAGS);
	idt_set_gate(8, (unsigned)isr8, IDT_SEG, ISR_FLAGS);
	idt_set_gate(9, (unsigned)isr9, IDT_SEG, ISR_FLAGS);
	idt_set_gate(10, (unsigned)isr10, IDT_SEG, ISR_FLAGS);
	idt_set_gate(11, (unsigned)isr11, IDT_SEG, ISR_FLAGS);
	idt_set_gate(12, (unsigned)isr12, IDT_SEG, ISR_FLAGS);
	idt_set_gate(13, (unsigned)isr13, IDT_SEG, ISR_FLAGS);
	idt_set_gate(14, (unsigned)isr14, IDT_SEG, ISR_FLAGS);
	idt_set_gate(15, (unsigned)isr15, IDT_SEG, ISR_FLAGS);
	idt_set_gate(16, (unsigned)isr16, IDT_SEG, ISR_FLAGS);
	idt_set_gate(17, (unsigned)isr17, IDT_SEG, ISR_FLAGS);
	idt_set_gate(18, (unsigned)isr18, IDT_SEG, ISR_FLAGS);
	idt_set_gate(19, (unsigned)isr19, IDT_SEG, ISR_FLAGS);
	idt_set_gate(20, (unsigned)isr20, IDT_SEG, ISR_FLAGS);
	idt_set_gate(21, (unsigned)isr21, IDT_SEG, ISR_FLAGS);
	idt_set_gate(22, (unsigned)isr22, IDT_SEG, ISR_FLAGS);
	idt_set_gate(23, (unsigned)isr23, IDT_SEG, ISR_FLAGS);
	idt_set_gate(24, (unsigned)isr24, IDT_SEG, ISR_FLAGS);
	idt_set_gate(25, (unsigned)isr25, IDT_SEG, ISR_FLAGS);
	idt_set_gate(26, (unsigned)isr26, IDT_SEG, ISR_FLAGS);
	idt_set_gate(27, (unsigned)isr27, IDT_SEG, ISR_FLAGS);
	idt_set_gate(28, (unsigned)isr28, IDT_SEG, ISR_FLAGS);
	idt_set_gate(29, (unsigned)isr29, IDT_SEG, ISR_FLAGS);
	idt_set_gate(30, (unsigned)isr30, IDT_SEG, ISR_FLAGS);
	idt_set_gate(31, (unsigned)isr31, IDT_SEG, ISR_FLAGS);*/
}
