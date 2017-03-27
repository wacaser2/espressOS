#include "paging.h"
#include "lib.h"
#include "multiboot.h"


uint32_t page_directory[oneUNIT] __attribute__((aligned(oneUNIT * PAGE_SIZE_MULTIPLIER)));	// 1024 entries. all 4kB aligned
uint32_t first_page_table[oneUNIT] __attribute__((aligned(oneUNIT * PAGE_SIZE_MULTIPLIER)));



/*
 * paging_init
 *   DESCRIPTION: Initializes paging
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Creates a page directory and a 4mb page and
 *         		   multiple 4kb pages
 */
void
paging_init(unsigned long addr) {
	int i;
	for (i = 0; i < oneUNIT; i++)
	{
		page_directory[i] = 0;	// sets not present
		first_page_table[i] = (i << OFFSET); // setting indexs
	}

	first_page_table[VIDEO >> OFFSET] |= RW | PRESENT;
	page_directory[0] = ((uint32_t)first_page_table) | PRESENT;	// add page table to directory of 4kb pages
	page_directory[1] = KERNEL_ADDR | PRESENT | fourMBpage | RW;	// 4mb page

	// cr3 - PDBR register, holds page directory location
	// cr4 - 0 for 4kb and 1 for 4mb
	// cr0 - 1 to enable paging
	asm volatile (
		"movl %0,%%eax; \n \t"
		"movl %%eax,%%cr3; \n \t"
		"movl %%cr4,%%eax; \n \t"
		"orl  $0x00000010,%%eax; \n \t"
		"movl %%eax,%%cr4; \n \t"
		"movl %%cr0,%%eax; \n \t"
		"orl  $0x80000000,%%eax; \n \t"
		"movl %%eax,%%cr0; \n \t"
		: /* no outputs */
	: "r"(page_directory)
		: "eax"
		);
}
