

#ifndef IDT_H
#define IDT_H

//constants
#define IDT_SEG 0x08
#define ISR_FLAGS 0x8E

typedef struct regs {
    unsigned int32_t gs, fs, es, ds;      /* pushed the segs last */
    unsigned int32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int32_t int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int32_t eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
} isr_stack_t;

/* Interrupt Service Routines 
	First 32 are reserved for 
	exceptions. */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

void isrs_install();
void fault_handler(isr_stack_t *s);
void idt_set_gate(unsigned char index, unsigned int32_t offset, unsigned short seg, unsigned char flags);



#endif