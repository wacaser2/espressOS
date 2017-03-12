#ifndef _KEYBOARD_H
#define _KEYBOARD_H


/* Ports that Keyboard sits on */
#define KEYBOARD_PORT 0x60

/* Irq that the Keyboard sits on */
#define KEYBOARD_IRQ 1

/* Initialize both Keyboard */
void keyboard_init();

/* handles keyboard input by echoing char pressed to the screen */
void keyboard_handler();

// keyboard handler
/*
keyboard_handler_wrapper:
	cli
	pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax	# Load the Kernel Data Segment descriptor!
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax   # Push us the stack
    push %eax
    call keyboard_handler
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!	


*/

#endif /* _KEYBOARD_H */