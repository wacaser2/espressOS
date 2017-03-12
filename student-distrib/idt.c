/*File for all of the functions for idt*/

#include "x86_desc.h"
#include "lib.h"

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

/* System Calls: */
int32_t halt(uint8_t status){
	return 0;
}
int32_t execute(const uint8_t* command){
	return 0;
}
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
	return 0;
}
int32_t open(const uint8_t* filename){
	return 0;
}
int32_t close(int32_t fd){
	return 0;
}
int32_t getargs(uint8_t* buf, int32_t nbytes){
	return 0;
}
int32_t vidmap(uint8_t** screen_start){
	return 0;
}
int32_t set_handler(int32_t signum, void* handler_address){
	return 0;
}
int32_t sigreturn(void){
	return 0;
}

/* Function to print the exception messages */
void fault_handler(isr_stack_t *s){
	if(s->int_no < 32){
		puts(exception_messages[s->int_no]);
		puts("\n");
		for(;;);
	}
}


/* Function to install all of the Interrupt Service Routines */
void isrs_install(){
	SET_IDT_ENTRY(idt[0], (unsigned)isr0);
	SET_IDT_ENTRY(idt[1], (unsigned)isr1);
	SET_IDT_ENTRY(idt[2], (unsigned)isr2);
	SET_IDT_ENTRY(idt[3], (unsigned)isr3);
	SET_IDT_ENTRY(idt[4], (unsigned)isr4);
	SET_IDT_ENTRY(idt[5], (unsigned)isr5);
	SET_IDT_ENTRY(idt[6], (unsigned)isr6);
	SET_IDT_ENTRY(idt[7], (unsigned)isr7);
	SET_IDT_ENTRY(idt[8], (unsigned)isr8);
	SET_IDT_ENTRY(idt[9], (unsigned)isr9);
	SET_IDT_ENTRY(idt[10], (unsigned)isr10);
	SET_IDT_ENTRY(idt[11], (unsigned)isr11);
	SET_IDT_ENTRY(idt[12], (unsigned)isr12);
	SET_IDT_ENTRY(idt[13], (unsigned)isr13);
	SET_IDT_ENTRY(idt[14], (unsigned)isr14);
	SET_IDT_ENTRY(idt[15], (unsigned)isr15);
	SET_IDT_ENTRY(idt[16], (unsigned)isr16);
	SET_IDT_ENTRY(idt[17], (unsigned)isr17);
	SET_IDT_ENTRY(idt[18], (unsigned)isr18);
	SET_IDT_ENTRY(idt[19], (unsigned)isr19);
	SET_IDT_ENTRY(idt[20], (unsigned)isr20);
	SET_IDT_ENTRY(idt[21], (unsigned)isr21);
	SET_IDT_ENTRY(idt[22], (unsigned)isr22);
	SET_IDT_ENTRY(idt[23], (unsigned)isr23);
	SET_IDT_ENTRY(idt[24], (unsigned)isr24);
	SET_IDT_ENTRY(idt[25], (unsigned)isr25);
	SET_IDT_ENTRY(idt[26], (unsigned)isr26);
	SET_IDT_ENTRY(idt[27], (unsigned)isr27);
	SET_IDT_ENTRY(idt[28], (unsigned)isr28);
	SET_IDT_ENTRY(idt[29], (unsigned)isr29);
	SET_IDT_ENTRY(idt[30], (unsigned)isr30);
	SET_IDT_ENTRY(idt[31], (unsigned)isr31);
	//SET_IDT_ENTRY(idt[0x21], (unsigned)keyboard_handler);
	SET_IDT_ENTRY(idt[0x28], (unsigned)rtc_handler);
	SET_IDT_ENTRY(idt[0x80], (unsigned)sys_call);
}
