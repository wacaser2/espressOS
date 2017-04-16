
#include "lib.h"
#include "x86_desc.h"
#include "syscalls.h"
#include "paging.h"

uint8_t process_num[6] = { 0, 0, 0, 0, 0, 0 };
volatile int process = -1;

fops_tbl_t stdin_ops = { (void*)null_ops, terminal_read, (void*)null_ops, (void*)null_ops };
fops_tbl_t stdout_ops = (fops_tbl_t) { (void*)null_ops, (void*)null_ops, terminal_write, (void*)null_ops };
fops_tbl_t rtc_ops = (fops_tbl_t) { rtc_open, rtc_read, rtc_write, rtc_close };
fops_tbl_t file_ops = (fops_tbl_t) { file_open, file_read, file_write, file_close };
fops_tbl_t dir_ops = (fops_tbl_t) { dir_open, dir_read, dir_write, dir_close };
fops_tbl_t default_ops = (fops_tbl_t) { (void*)null_ops, (void*)null_ops, (void*)null_ops, (void*)null_ops };

/* System Calls*/

int32_t halt(uint8_t status) {
	pcb_t * block = (pcb_t *)(eightMB - (process + 1) * eightKB);
	process_num[process] = ZERO;

	VtoPmap(onetwentyeightMB, (eightMB + (process * fourMB)));
	process = block->parent_block->process_id;
	tss.esp0 = block->parent_ksp;

	int i;
	for (i = 0; i < MAXFILES; i++) {
		block->fdarray[i].fops_tbl_pointer = (void*)null_ops;
		block->fdarray[i].flags = ZERO;
	}
	return 0;
}

int32_t execute(const uint8_t* command) {
	int start = 0;
	while (command[start] == ' ') {
		start++;
	}
	int end = start;
	while (command[end] != ' ') {
		end++;
	}
	uint8_t name[32];
	uint8_t restarg[128];
	int i, j;
	for (i = 0, j = start; (i < (end - start)) || (j < end); i++, j++)
	{
		name[i] = command[j];				// extract name of file
	}
	

	name[i] = '\0';			// null-terminated
	end++;
	start = end;
	while (command[end] != '\0') {
		end++;
	}
	for (i = 0, j = start; (i < (end - start)) || (j < end); i++, j++)
	{
		restarg[i] = command[j];			// extract the rest of the argument
	}
	restarg[i] = '\0';		// null-terminated

	puts(name);
	putc('\n');
	puts(restarg);
	putc('\n');

	dentry_t * dentry;
	if (read_dentry_by_name((int8_t*)name, dentry) == -1) {
		return -1;
	}

	int8_t buffer[4];
	int8_t magic_number[4] = { 0x7f, 0x45, 0x4c, 0x46 };		// DEL, E, L, F
	read_data(dentry->inode_index, 0, (uint8_t*)buffer, 4);
	if (strncmp(buffer, magic_number, 4) != 0) {					// if you don't get the magic numbers in the first four bytes
		return -1;
	}

	uint32_t entry_point;									// entry point to read from
	read_data(dentry->inode_index, 24, (uint8_t*)&entry_point, 4);

	for (i = 0; i < MAXPROCESSES; i++) {
		if (process_num[i] == ZERO) {
			process_num[i] = ONE;
			process = i;
			break;
		}
	}
	if (process < 0) {
		return -1;
	}

	VtoPmap(onetwentyeightMB, (eightMB + (process * fourMB)));

	read_data(dentry->inode_index, 0, (void*)0x8048000, get_inode_length(dentry->inode_index));

	pcb_t * block = (pcb_t *)(eightMB - (process + 1) * eightKB);	// top of the 8KB stack
	block->process_id = process;

	if (process == 0) {						// first process, so set parent to itself
		block->parent_block = (pcb_t *)(eightMB - (process + 1) * eightKB);
	}
	else {
		block->parent_block = (pcb_t *)(eightMB - process*eightKB);
		block->parent_ksp = tss.esp0;
		block->parent_kbp = tss.ebp;
	}

	// /* Save the kernel stack pointer and kernel base pointer of the parent process control block here as members of the pcb struct because we will need it for the halt function */


	/* STDIN */
	block->fdarray[ZERO].fops_tbl_pointer = &stdin_ops;
	block->fdarray[ZERO].inode = -1;					// or 0?
	block->fdarray[ZERO].file_pos = FILE_START_POS;
	block->fdarray[ZERO].flags = ONE;					// in use
	/* STDOUT */
	block->fdarray[ONE].fops_tbl_pointer = &stdout_ops;
	block->fdarray[ONE].inode = -1;
	block->fdarray[ONE].file_pos = FILE_START_POS;
	block->fdarray[ONE].flags = ONE;

	/* Intialize the remaining files to the default values */
	for (i = 2; i < MAXFILES; i++) {
		block->fdarray[i].fops_tbl_pointer = &default_ops;
		block->fdarray[i].inode = -1;
		block->fdarray[i].file_pos = FILE_START_POS;
		block->fdarray[i].flags = ZERO;					// not in use
	}

	//tss.ss0 = KERNEL_DS;
	tss.esp0 = eightMB - (process * eightKB) - 4;

	uint32_t tmp = 0x83FFFFC/*(eightMB + ((process + 1) * fourMB) - 4)*/;
	asm volatile (
		"pushl $0x2B; \n"
		//"movw 0x2B, %%ds \n"
		"pushl %0; \n"
		"pushfl; \n"
		"popl %%ecx; \n"
		"orl $0x0200, %%ecx; \n"
		"pushl %%ecx; \n"
		"pushl $0x23; \n"
		"pushl %1; \n"
		"iret; \n"
		: /* outputs */
	: "r" (tmp), "r" (entry_point) /* inputs  */
		: "ecx" // clobbers
		);


	// asm volatile (
	//  mov ax,0x23
 //     mov ds,ax
 //     mov es,ax 
 //     mov fs,ax 
 //     mov gs,ax ;we don't need to worry about SS. it's handled by iret
 
 //     mov eax,esp
 //     push 0x23 ;user data segment with bottom 2 bits set for ring 3
 //     push eax ;push our current stack just for the heck of it
 //     pushf
 //     push 0x1B; ;user code segment with bottom 2 bits set for ring 3
 //     push _test_user_function ;may need to remove the _ for this to work right 
 //     iret
 //     );

	return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes) {
	if (fd < 0 || fd >= MAXFILES || buf == NULL) {
		return -1;
	}
	pcb_t * block = (pcb_t *)(eightMB - (process + 1) * eightKB);
	if (block->fdarray[fd].flags == ZERO) {
		return -1;
	}
	return block->fdarray[fd].fops_tbl_pointer->read(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
	if (fd < 0 || fd >= MAXFILES || buf == NULL) {
		return -1;
	}
	pcb_t * block = (pcb_t *)(eightMB - (process + 1) * eightKB);
	if (block->fdarray[fd].flags == ZERO) {
		return -1;
	}
	return block->fdarray[fd].fops_tbl_pointer->write(fd, buf, nbytes);
}

int32_t open(const uint8_t* filename) {
	pcb_t * block = (pcb_t *)(eightMB - (process + 1) * eightKB);
	dentry_t * dentry;
	if (read_dentry_by_name((int8_t*)filename, dentry)) {
		return -1;
	}

	int i;
	for (i = 2; i < MAXFILES; i++) {
		if (block->fdarray[i].flags == ZERO) {
			block->fdarray[i].flags = ONE;
			break;
		}
	}
	/* Check if we ran out of descriptors*/
	if (i == MAXFILES) {
		return -1;
	}

	block->fdarray[i].file_pos = FILE_START_POS;
	if (dentry->file_type == RTC_TYPE) {
		block->fdarray[i].fops_tbl_pointer = &rtc_ops;
	}
	else if (dentry->file_type == DIR_TYPE) {
		block->fdarray[i].fops_tbl_pointer = &dir_ops;
	}
	else if (dentry->file_type == FILE_TYPE) {
		block->fdarray[i].fops_tbl_pointer = &file_ops;
	}

	//return block->fdarray[i]->fops_tbl_pointer->open(filename);
	return i; // file descriptor is returned
}

int32_t close(int32_t fd) {
	pcb_t * block = (pcb_t *)(eightMB - (process + 1) * eightKB);
	if (fd >= 2 && fd < MAXFILES && block->fdarray[fd].flags == ONE) {
		block->fdarray[fd].flags = ZERO;
	}
	else {
		return -1;
	}

	return block->fdarray[fd].fops_tbl_pointer->close(fd);
}

int32_t getargs(uint8_t* buf, int32_t nbytes) {
	return 0;
}

int32_t vidmap(uint8_t** screen_start) {
	return 0;
}

int32_t set_handler(int32_t signum, void* handler_address) {
	return 0;
}

int32_t sigreturn(void) {
	return 0;
}

int32_t null_ops(void) {
	return -1;
}
