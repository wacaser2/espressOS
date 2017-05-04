# isr.S the isr routine front and back stubs

# define ASM		1

#include "x86_desc.h"
#include "idt.h"


# Global variables for the isrs
.text
.globl isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, isr8, isr9
.globl isr10, isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19
.globl isr20, isr21, isr22, isr23, isr24, isr25, isr26, isr27, isr28, isr29
.globl isr30, isr31
.globl rtc_handler_wrapper, sys_call, keyboard_handler_wrapper, pit_handler_wrapper, mouse_handler_wrapper


# isr0-31
#   DESCRIPTION: Initializes paging
#   INPUTS: none
#   OUTPUTS: none
#   RETURN VALUE: none
#   SIDE EFFECTS: Pushes the an error code if one is not provided and then pushes isr # and jumps to the common handler
isr0:
	cli
	pushl $0x00
	pushl $0x00
	jmp isr_common
isr1:
	cli
	pushl $0x00
	pushl $0x01
	jmp isr_common	
isr2:
	cli
	pushl $0x00
	pushl $0x02
	jmp isr_common
isr3 :
	cli
	pushl $0x00
	pushl $0x03
	jmp isr_common
isr4 :
	cli
	pushl $0x00
	pushl $0x04
	jmp isr_common
isr5 :
	cli
	pushl $0x00
	pushl $0x05
	jmp isr_common
isr6 :
	cli
	pushl $0x00
	pushl $0x06
	jmp isr_common
isr7 :
	cli
	pushl $0x00
	pushl $0x07
	jmp isr_common
isr8 :
	cli
	pushl $0x08
	jmp isr_common
isr9 :
	cli
	pushl $0x00
	pushl $0x09
	jmp isr_common
isr10 :
	cli
	pushl $0x0a
	jmp isr_common
isr11 :
	cli
	pushl $0x0b
	jmp isr_common
isr12 :
	cli
	pushl $0x0c
	jmp isr_common
isr13 :
	cli
	pushl $0x0d
	jmp isr_common
isr14 :
	cli
	pushl $0x0e
	jmp isr_common
isr15 :
	cli
	pushl $0x00
	pushl $0x0f
	jmp isr_common
isr16 :
	cli
	pushl $0x00
	pushl $0x10
	jmp isr_common
isr17 :
	cli
	pushl $0x11
	jmp isr_common
isr18 :
	cli
	pushl $0x00
	pushl $0x12
	jmp isr_common
isr19 :
	cli
	pushl $0x00
	pushl $0x13
	jmp isr_common
isr20 :
	cli
	pushl $0x00
	pushl $0x14
	jmp isr_common
isr21 :
	cli
	pushl $0x00
	pushl $0x15
	jmp isr_common
isr22 :
	cli
	pushl $0x00
	pushl $0x16
	jmp isr_common
isr23 :
	cli
	pushl $0x00
	pushl $0x17
	jmp isr_common
isr24 :
	cli
	pushl $0x00
	pushl $0x18
	jmp isr_common
isr25 :
	cli
	pushl $0x00
	pushl $0x19
	jmp isr_common
isr26 :
	cli
	pushl $0x00
	pushl $0x1a
	jmp isr_common
isr27 :
	cli
	pushl $0x00
	pushl $0x1b
	jmp isr_common
isr28 :
	cli
	pushl $0x00
	pushl $0x1c
	jmp isr_common
isr29 :
	cli
	pushl $0x00
	pushl $0x1d
	jmp isr_common
isr30 :
	cli
	pushl $0x1e
	jmp isr_common
isr31 :
	cli
	pushl $0x00
	pushl $0x1f
	jmp isr_common

# Very similar to an examble from osdev
# isr_common
#   DESCRIPTION: Calls the exception handler
#   INPUTS: none
#   OUTPUTS: none
#   RETURN VALUE: none
#   SIDE EFFECTS: saves all the registers
isr_common:
	pushal	# push the user regs
    pushl %ds	# push the segment regs, just in case
    pushl %es
    pushl %fs
    pushl %gs
    movl $KERNEL_CS, %eax	# Load the Kernel Code Segment descriptor, probably already was
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    pushl %esp
    call fault_handler	# call the fault handler
	popl %eax	# pop all of the segment regs
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal	# pop all the user regs
    addl $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!



# pit_handler_wrapper
#   DESCRIPTION: Saves registers and call pit_handler
#   INPUTS: none
#   OUTPUTS: none
#   RETURN VALUE: none
#   SIDE EFFECTS: Calls keyboard_handler so anything that does
pit_handler_wrapper:
	cli
	pushl %eax
	pushl %eax
	pushal	# push the user regs
    pushl %ds	# push the segment regs, just in case
    pushl %es
    pushl %fs
    pushl %gs
    movl $KERNEL_CS, %eax	# Load the Kernel Code Segment descriptor, probably already was
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    pushl %esp
    call pit_handler
	popl %eax	# pop all of the segment regs
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal	# pop all the user regs
    addl $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!	

# mouse_handler_wrapper
#   DESCRIPTION: Saves registers and call mouse_handler
#   INPUTS: none
#   OUTPUTS: none
#   RETURN VALUE: none
#   SIDE EFFECTS: Calls mouse_handler so anything that does
mouse_handler_wrapper:
	cli
	pushl %eax
	pushl %eax
	pushal	# push the user regs
    pushl %ds	# push the segment regs, just in case
    pushl %es
    pushl %fs
    pushl %gs
    movl $KERNEL_CS, %eax	# Load the Kernel Code Segment descriptor, probably already was
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    pushl %esp
    call mouse_handler
	popl %eax	# pop all of the segment regs
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal	# pop all the user regs
    addl $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!	
	
# keyboard_handler_wrapper
#   DESCRIPTION: Saves registers and call keyboard_handler
#   INPUTS: none
#   OUTPUTS: none
#   RETURN VALUE: none
#   SIDE EFFECTS: Calls keyboard_handler so anything that does
keyboard_handler_wrapper:
	cli
	pushl %eax
	pushl %eax
	pushal	# push the user regs
    pushl %ds	# push the segment regs, just in case
    pushl %es
    pushl %fs
    pushl %gs
    movl $KERNEL_CS, %eax	# Load the Kernel Code Segment descriptor, probably already was
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    pushl %esp
    call keyboard_handler
	popl %eax	# pop all of the segment regs
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal	# pop all the user regs
    addl $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret

# rtc_handler_wrapper
#   DESCRIPTION: Saves the registers and call rtc_handler
#   INPUTS: none
#   OUTPUTS: none
#   RETURN VALUE: none
#   SIDE EFFECTS: Calls rtc_handler so anything that does
rtc_handler_wrapper:
	cli
	pushl %eax
	pushl %eax
	pushal	# push the user regs
    pushl %ds	# push the segment regs, just in case
    pushl %es
    pushl %fs
    pushl %gs
    movl $KERNEL_CS, %eax	# Load the Kernel Code Segment descriptor, probably already was
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    pushl %esp
    call rtc_handler
	popl %eax	# pop all of the segment regs
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal	# pop all the user regs
    addl $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!


# Dispatcher for the system calls
# sys_call
#   DESCRIPTION: Takes the sys_call and jumps to the proper function
#   INPUTS: %eax - the number of the function to call
#   OUTPUTS: %eax
#   RETURN VALUE: -1 for an error, 0 else
#   SIDE EFFECTS: Calls system calls, so anything from system halt to opening a file
sys_call:
	cli
	pushl %eax
	pushl %eax
	pushal	# push the user regs
    pushl %ds	# push the segment regs, just in case
    pushl %es
    pushl %fs
    pushl %gs
	pushl %eax
    movl $KERNEL_CS, %eax	# Load the Kernel Code Segment descriptor, probably already was
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    popl %eax
	pushl %esp
	subl $1, %eax	# zero index
	cmpl $10, %eax
	jae sys_call_err # 0 or above 10 return error
	pushl %edx
	pushl %ecx
	pushl %ebx
	# sti # enable interrupts
	call *sys_call_jmp_table(,%eax,4)
	addl $16, %esp

sys_call_ret:
    mov %eax, return_value
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal	# pop all the user regs
    addl $8, %esp     # Cleans up the pushed error code and pushed ISR number
	movl return_value, %eax
	iret

return_value:
	.long	0

sys_call_err:
	popl %eax	# pop all of the segment regs
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal	# pop all the user regs
    addl $8, %esp     # Cleans up the pushed error code and pushed ISR number
	movl $-1, %eax
	iret

sys_call_jmp_table:
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
