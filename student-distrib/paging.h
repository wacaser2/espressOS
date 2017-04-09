
#ifndef _PAGING_H
#define _PAGING_H

#include "i8259.h"
#include "lib.h"
#include "keyboard.h"

#define oneUNIT 1024
#define PAGE_SIZE_MULTIPLIER 4
#define fourMBpage 0x0080
/* number of bits before the index to the page table*/
#define OFFSET 12

#define KERNEL_ADDR 0x0400000	// starting addr of kernel code

#define RW 0x00000002
#define PRESENT 0x00000001
#define USER	0x00000004

/* Function to initialize paging*/
void paging_init(unsigned long addr);

void VtoPmap(uint32_t vaddr, uint32_t paddr);

#endif
