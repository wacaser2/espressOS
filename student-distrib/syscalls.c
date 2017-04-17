
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

	/* Clear this program's process number */
	process_num[process] = ZERO;

	/* If this process is the top shell, restart */
	if (process == 0) {
		clear();
		execute((uint8_t *)"shell");
	}
	/* Reset process to parent */
	pcb_t * block = get_pcb();
	process = block->parent_block->process_id;

	/* Reset page mapping to parent process */
	VtoPmap(onetwentyeightMB, (eightMB + ((process)* fourMB)));

	/* Update tss */
	tss.esp0 = block->parent_ksp;

	/* Clear out fdarray */
	int32_t i;
	for (i = 0; i < MAXFILES; i++) {
		block->fdarray[i].fops_tbl_pointer = (void*)null_ops;
		block->fdarray[i].flags = ZERO;
	}

	/* Reset ebp and esp to the execute for this function to return to parent */
	asm volatile(
		"movl %0, %%ebp \n"
		"movl %1, %%esp \n"
		: : "r" (block->parent_kbp), "r" (block->parent_ksp)
		);

	return 0;
}

int32_t execute(const uint8_t* command) {


	/* Extract name */
	int32_t start = 0;
	while (command[start] == ' ')
		start++;
	int32_t end = start;
	while (command[end] != ' ' && command[end] != '\0' && end < (NAME_SIZE + start))
		end++;
	uint8_t name[NAME_SIZE + 1];
	strncpy((int8_t*)name, (int8_t*)(command + start), end - start);
	name[end] = '\0';

	/* Find file */
	dentry_t dentry;
	if (read_dentry_by_name((int8_t*)name, &dentry) == -1)
		return -1;

	/* Check for executable */
	int8_t buffer[INT_BYTES];
	int8_t magic_number[INT_BYTES] = { 0x7f, 0x45, 0x4c, 0x46 };		// DEL, E, L, F
	read_data(dentry.inode_index, 0, (uint8_t*)buffer, INT_BYTES);
	if (strncmp(buffer, magic_number, INT_BYTES) != 0) 					// if you don't get the magic numbers in the first four bytes
		return -1;

	/* Find process number */
	process = -1;
	int32_t i;
	for (i = 0; i < MAXPROCESSES; i++) {
		if (process_num[i] == ZERO) {
			process_num[i] = ONE;
			process = i;
			break;
		}
	}
	if (process < 0) {
		process = MAXPROCESSES - 1;	// so it doesnt exceed 
		puts("Maximum number of processess running\n");
		return -1;
	}

	/* Set up PCB */
	pcb_t * block = get_pcb();	// top of the 8KB stack
	block->process_id = process;
	if (process == 0)			// first process, so set parent to itself
		block->parent_block = block;
	else
		block->parent_block = (pcb_t *)(eightMB - process * eightKB);
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

	/* Extract args */
	start = end;
	while (command[start] == ' ')
		start++;
	end = start;
	while (command[end] != '\0')
		end++;
	strncpy((int8_t*)block->args, (int8_t*)(command + start), end - start + 1);

	/* Set up paging */
	VtoPmap(onetwentyeightMB, (eightMB + (process * fourMB)));

	/* Find entry point to program */
	uint32_t entry_point;									// entry point to read from
	read_data(dentry.inode_index, 24, (uint8_t*)&entry_point, INT_BYTES);

	/* Load program */
	read_data(dentry.inode_index, 0, (void*)PROG_START, get_inode_length(dentry.inode_index));

	/* Save the kernel stack pointer and kernel base pointer of the parent process control block
	here as members of the pcb struct because we will need it for the halt function */
	asm volatile(
		"movl %%ebp, %0 \n"
		"movl %%esp, %1 \n"
		:"=r"(block->parent_kbp), "=r"(block->parent_ksp)
		);

	/* Update tss */
	tss.esp0 = eightMB - (process * eightKB) - 4;

	/* Context Switch */
	asm volatile (
		"pushl $0x2B; \n"
		"pushl %0; \n"
		"pushfl; \n"
		"popl %%ecx; \n"
		"orl $0x0200, %%ecx; \n"
		"pushl %%ecx; \n"
		"pushl $0x23; \n"
		"pushl %1; \n"
		"iret; \n"
		: /* outputs */ : "r" (PROG_STACK_START), "r" (entry_point) /* inputs  */ : "ecx" // clobbers
		);

	return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes) {

	/* Check invalid fd */
	if (fd < 0 || fd >= MAXFILES || buf == NULL)
		return -1;
	pcb_t * block = get_pcb();
	if (block->fdarray[fd].flags == ZERO)
		return -1;

	/* Return read for the file type */
	return block->fdarray[fd].fops_tbl_pointer->read(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {

	/* Check invalid fd */
	if (fd < 0 || fd >= MAXFILES || buf == NULL)
		return -1;
	pcb_t * block = get_pcb();
	if (block->fdarray[fd].flags == ZERO)
		return -1;

	/* Return write for the file type */
	return block->fdarray[fd].fops_tbl_pointer->write(fd, buf, nbytes);
}

int32_t open(const uint8_t* filename) {

	/* Check for file existence */
	dentry_t dentry;
	if (read_dentry_by_name((int8_t*)filename, &dentry))
		return -1;

	/* Find fd number */
	pcb_t * block = get_pcb();
	int32_t i;
	for (i = 2; i < MAXFILES; i++) {
		if (block->fdarray[i].flags == ZERO) {
			block->fdarray[i].flags = ONE;
			break;
		}
	}

	/* Check if we ran out of descriptors*/
	if (i == MAXFILES)
		return -1;

	/* Open files */
	block->fdarray[i].file_pos = FILE_START_POS;
	if (dentry.file_type == RTC_TYPE)
		block->fdarray[i].fops_tbl_pointer = &rtc_ops;
	else if (dentry.file_type == DIR_TYPE)
		block->fdarray[i].fops_tbl_pointer = &dir_ops;
	else if (dentry.file_type == FILE_TYPE) {
		block->fdarray[i].fops_tbl_pointer = &file_ops;
		block->fdarray[i].inode = dentry.inode_index;
	}

	//return block->fdarray[i]->fops_tbl_pointer->open(filename);
	return i; // file descriptor is returned
}

int32_t close(int32_t fd) {

	/* Check for fd */
	pcb_t * block = get_pcb();
	if (fd >= 2 && fd < MAXFILES && block->fdarray[fd].flags == ONE)
		block->fdarray[fd].flags = ZERO;
	else		// non-existent fd
		return -1;

	/* Return close for that file type */
	return block->fdarray[fd].fops_tbl_pointer->close(fd);
}

int32_t getargs(uint8_t* buf, int32_t nbytes) {

	pcb_t* block = get_pcb();

	/* Check if enough space in buf */
	if (nbytes < strlen((int8_t*)block->args))
		return -1;

	/* Copy to buf */
	strcpy((int8_t*)buf, (int8_t*)block->args);

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

pcb_t* get_pcb() {
	return (pcb_t *)(eightMB - (process + 1) * eightKB);
}

/* For debugging */
void implicit_proc() {
	process = 0;
	process_num[0] = ONE;

	pcb_t* block = (pcb_t*)(eightMB - eightKB);

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
}