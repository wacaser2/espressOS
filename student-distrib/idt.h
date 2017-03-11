//
//
//#ifndef _IDT_H
//#define _IDT_H
//
//#include "types.h"
//
////constants
//#define IDT_SEG 0x08
//#define ISR_FLAGS 0x8E
//
///* Interrupt Service Routines
//	First 32 are reserved for
//	exceptions. */
//extern void isr0();
//extern void isr1();
//
//void isrs_install();
//void fault_handler(isr_stack_t *s);
//void idt_set_gate(unsigned char index, uint32_t offset, unsigned short seg, unsigned char flags);
//
//
//
//#endif
