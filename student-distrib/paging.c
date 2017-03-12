
#include "paging.h"



uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void
paging_init(void){
	int i;
	for(i = 0; i < 1024; i++)
	{
		page_directory[i] = 0x00000002;
		first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present
	}

	page_directory[0] = ((unsigned int)first_page_table) | 3;

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

   