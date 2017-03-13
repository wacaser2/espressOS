
#ifndef _PAGING_H
#define _PAGING_H

#include "i8259.h"
#include "lib.h"
#include "keyboard.h"

#define oneUNIT 1024
#define PAGE_SIZE_MULTIPLIER 4
#define fourMBpage 0x0080

#define KERNEL_ADDR 0x0400000	// starting addr of kernel code

#define RW 0x00000002
#define PRESENT 0x00000001

void paging_init(void);

#endif
