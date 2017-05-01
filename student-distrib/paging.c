#include "paging.h"
#include "lib.h"
#include "multiboot.h"
#include "syscalls.h"


uint32_t page_directory[oneUNIT] __attribute__((aligned(oneUNIT * PAGE_SIZE_MULTIPLIER)));	// 1024 entries. all 4kB aligned
uint32_t first_page_table[oneUNIT] __attribute__((aligned(oneUNIT * PAGE_SIZE_MULTIPLIER)));
uint32_t window_page_table[oneUNIT] __attribute__((aligned(oneUNIT * PAGE_SIZE_MULTIPLIER)));
uint32_t user_page_table[oneUNIT] __attribute__((aligned(oneUNIT * PAGE_SIZE_MULTIPLIER)));



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
		first_page_table[i] = (i << OFFSET); // setting indexes
		window_page_table[i] = (2 + MAXPROCESSES)*fourMB + (i << OFFSET); // setting indexes
	}

	first_page_table[VIDEO >> OFFSET] |= RW | PRESENT;
	first_page_table[(VIDEO >> OFFSET) - 1] = first_page_table[VIDEO >> OFFSET];
	page_directory[0] = ((uint32_t)first_page_table) | PRESENT | RW;	// add page table to directory of 4kb pages
	page_directory[1] = KERNEL_ADDR | PRESENT | fourMBpage | RW;	// 4mb page
	page_directory[2 + MAXPROCESSES] = ((uint32_t)window_page_table) | PRESENT | RW;	// add page table to directory of 4kb pages
	first_page_table[(int32_t)page_directory >> OFFSET] |= RW | PRESENT;

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
	: "r"(page_directory)		// input operands
		: "eax"						// clobbers
		);
}

void windowPage(int32_t proc) {
	window_page_table[(proc)] |= RW | PRESENT;
	clearTLB();
}

void videoPage(void* window) {
	first_page_table[(VIDEO >> OFFSET)] = (((int32_t)window) & 0xFFFFF000) | RW | PRESENT;
	VtoPpage(onetwentyeightMB + fourMB, (uint32_t)window);
	clearTLB();
}

void VtoPpage(uint32_t vaddr, uint32_t paddr) {
	page_directory[((vaddr >> 22) & 0x000003FF)] = ((uint32_t)user_page_table) | PRESENT | RW | USER;
	user_page_table[((vaddr >> 12) & 0x000003FF)] = (paddr & 0xFFFFF000) | PRESENT | RW | USER;
	clearTLB();
}

void VtoPmap(uint32_t vaddr, uint32_t paddr) {
	int32_t vidx = (vaddr >> 22) & 0x000003FF;		// vidx = 32
	page_directory[vidx] = (paddr & 0xFFC00000) | PRESENT | fourMBpage | RW | USER;
	clearTLB();
}

void clearTLB() {
	asm volatile (
		"movl %%cr3,%%eax; \n \t"
		"movl %%eax,%%cr3; \n \t"
		: /* no outputs */
	: /* no inputs  */
		: "eax"					// clobbers
		);
}
