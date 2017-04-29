
#include "lib.h"
#include "x86_desc.h"
#include "syscalls.h"
#include "paging.h"
#include "window.h"

uint8_t process_num[6] = { 0, 0, 0, 0, 0, 0 };
volatile int32_t process = -1;
int32_t process_term = -1;
int32_t active = -1;
int32_t terminal_process[3] = { -1,-1,-1 };

fops_tbl_t stdin_ops = { (void*)null_ops, terminal_read, (void*)null_ops, (void*)null_ops };
fops_tbl_t stdout_ops = (fops_tbl_t) { (void*)null_ops, (void*)null_ops, terminal_write, (void*)null_ops };
fops_tbl_t rtc_ops = (fops_tbl_t) { rtc_open, rtc_read, rtc_write, rtc_close };
fops_tbl_t file_ops = (fops_tbl_t) { file_open, file_read, file_write, file_close };
fops_tbl_t dir_ops = (fops_tbl_t) { dir_open, dir_read, dir_write, dir_close };
fops_tbl_t default_ops = (fops_tbl_t) { (void*)null_ops, (void*)null_ops, (void*)null_ops, (void*)null_ops };

void switch_active(int32_t term) {
	process_term = active = term;
	switch_process(term);
}

void switch_process(int32_t term) {
	/*store current*/
	if (process != -1) {
		pcb_t* block = get_pcb(get_proc());
		asm volatile(
			"movl %%ebp, %0 \n"
			"movl %%esp, %1 \n"
			: "=r" (block->kbp), "=r" (block->ksp)
			);
	}

	process = terminal_process[term];
	if (active == term) {
		videoPage((void*)VIDEO);
		if (process_term == active)
			updateWindow(get_window(terminal_process[term]));
	}
	else
		videoPage((void*)get_window(process));
	process_term = term;

	if (process == -1) {
		execute((uint8_t*)"shell");
	}

	/*restore other*/
	VtoPmap(onetwentyeightMB, (eightMB + (process * fourMB)));
	tss.esp0 = eightMB - (process * eightKB) - 4;
	pcb_t* block = get_pcb(process);
	asm volatile(
		"movl %0, %%ebp \n"
		"movl %1, %%esp \n"
		: : "r" (block->kbp), "r" (block->ksp)
		);
}

/* System Calls*/

int32_t halt(uint8_t status) {

	/* Clear this program's process number */
	process_num[process] = ZERO;

	/* If this process is the top shell, restart */
	if (get_pcb(process)->parent_id == -1) {
		clear();
		terminal_process[process_term] = -1;
		process = -1;
		execute((uint8_t *)"shell");
	}

	if (!strncmp((int8_t*)get_pcb(process)->name, (int8_t*)"shell", NAME_SIZE))
		window_exit(process);

	/* Reset process to parent */
	pcb_t * block = get_pcb(process);
	process = block->parent_block->process_id;
	terminal_process[process_term] = process;
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
	int32_t ret_val;
	if (status == 255)
		ret_val = EXCEPTION_CODE;
	else if (status != 0)
		ret_val = 1;
	else
		ret_val = 0;

	/* Reset ebp and esp to the execute for this function to return to parent */
	asm volatile(
		"movl %0, %%eax \n"
		"movl %1, %%ebp \n"
		"movl %2, %%esp \n"
		"leave \n"
		"ret \n"
		: : "r" (ret_val), "r" (block->parent_kbp), "r" (block->parent_ksp)
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
	name[end - start] = '\0';

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
	int32_t parent_proc = process;
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
		process = parent_proc;	// so it doesnt exceed 
		puts("Maximum number of processess running\n");
		return 0;
	}

	/* Set up PCB */
	pcb_t * block = get_pcb(process);	// top of the 8KB stack
	block->process_id = process;
	block->parent_id = parent_proc;
	strcpy(block->command, "");
	block->key_idx = 0;
	block->cycles = 2;
	if (parent_proc == -1)			// first process, so set parent to itself
		block->parent_block = block;
	else
		block->parent_block = get_pcb(parent_proc);
	terminal_process[process_term] = process;
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

	/* Save name */
	strncpy((int8_t*)block->name, (int8_t*)name, NAME_SIZE);

	/* Extract args */
	start = end;
	while (command[start] == ' ')
		start++;
	end = start;
	while (command[end] != '\0' && command[end] != ' ')
		end++;
	strncpy((int8_t*)block->args, (int8_t*)(command + start), end - start);
	block->args[end - start] = '\0';

	/* Set up paging */
	VtoPmap(onetwentyeightMB, (eightMB + (process * fourMB)));
	if (!strncmp((int8_t*)name, (int8_t*)"shell", NAME_SIZE)) {
		block->window_id = process;
		window_init(process);
	}
	else {
		block->window_id = block->parent_block->window_id;
	}

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
	pcb_t * block = get_pcb(process);
	if (block->fdarray[fd].flags == ZERO)
		return -1;

	/* Return read for the file type */
	return block->fdarray[fd].fops_tbl_pointer->read(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {

	/* Check invalid fd */
	if (fd < 0 || fd >= MAXFILES || buf == NULL)
		return -1;
	pcb_t * block = get_pcb(process);
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
	pcb_t * block = get_pcb(process);
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

	block->fdarray[i].fops_tbl_pointer->open(filename);
	return i; // file descriptor is returned
}

int32_t close(int32_t fd) {

	/* Check for fd */
	pcb_t * block = get_pcb(process);
	if (fd >= 2 && fd < MAXFILES && block->fdarray[fd].flags == ONE)
		block->fdarray[fd].flags = ZERO;
	else		// non-existent fd
		return -1;

	/* Return close for that file type */
	return block->fdarray[fd].fops_tbl_pointer->close(fd);
}

int32_t getargs(uint8_t* buf, int32_t nbytes) {

	pcb_t* block = get_pcb(process);

	/* Check if enough space in buf */
	int32_t l = strlen((int8_t*)block->args);
	if (!l || nbytes < l)
		return -1;

	/* Copy to buf */
	strcpy((int8_t*)buf, (int8_t*)block->args);

	return 0;
}

int32_t vidmap(uint8_t** screen_start) {
	if (((int32_t)screen_start < onetwentyeightMB) || ((int32_t)screen_start>(onetwentyeightMB + fourMB)))
		return -1;
	uint32_t idx = onetwentyeightMB + fourMB;
	//VtoPpage(idx, VIDEO);
	*screen_start = (uint8_t*)idx;
	return idx;
}

int32_t set_handler(int32_t signum, void* handler_address) {
	return -1;
}

int32_t sigreturn(void) {
	return -1;
}

int32_t null_ops(void) {
	return -1;
}

pcb_t* get_pcb(int32_t proc) {
	return (pcb_t *)(eightMB - (proc + 1) * eightKB);
}

pcb_t* get_parent_pcb(int32_t proc) {
	return get_pcb(proc)->parent_block;
}

int32_t get_proc() {
	return process;
}

int32_t get_term_proc(int32_t term) {
	return terminal_process[term];
}

int32_t get_active() {
	return active;
}

int32_t get_proc_term() {
	return process_term;
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
