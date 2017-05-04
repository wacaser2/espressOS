
#include "lib.h"
#include "x86_desc.h"
#include "syscalls.h"
#include "paging.h"
#include "window.h"
#include "bootup.h"

/* Variables to keep track of running processes */
uint8_t process_num[MAXPROCESSES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
volatile int32_t process = -1;
int32_t process_term = -1;
int32_t active = -1;
int32_t terminal_process[3] = { -1,-1,-1 };
int32_t reload = 0;

/* Tables for the different file types */
fops_tbl_t stdin_ops = { (void*)null_ops, terminal_read, (void*)null_ops, (void*)null_ops };
fops_tbl_t stdout_ops = (fops_tbl_t) { (void*)null_ops, (void*)null_ops, terminal_write, (void*)null_ops };
fops_tbl_t rtc_ops = (fops_tbl_t) { rtc_open, rtc_read, rtc_write, rtc_close };
fops_tbl_t file_ops = (fops_tbl_t) { file_open, file_read, file_write, file_close };
fops_tbl_t dir_ops = (fops_tbl_t) { dir_open, dir_read, dir_write, dir_close };
fops_tbl_t default_ops = (fops_tbl_t) { (void*)null_ops, (void*)null_ops, (void*)null_ops, (void*)null_ops };

/*
void switch_active
input: int32_t term = terminal to switch to active
output: none
purpose: triggers the switching to that terminal
*/
void switch_active(int32_t term) {
	cli();
	process_term = active = term;
	reload = 1;
	switch_process(term);
}

/*
void switch_process
input: int32_t term
output: none
purpose: trigger the switching to a different process
*/
void switch_process(int32_t term) {
	cli();
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
	process_term = term;
	if (active == term) {
		videoPage((void*)VIDEO);
		if (reload)
		{
			updateWindow(get_window(terminal_process[term]));
			reload = 0;
		}
	}
	else
		videoPage((void*)get_window(process));

	if (process == -1) {
		execute((uint8_t*)"shell");
	}

	/*restore other*/
	VtoPmap(onetwentyeightMB, (eightMB + (process * fourMB)));
	pcb_t* block = get_pcb(process);
	/* return */
	tss.esp0 = eightMB - (process * eightKB) - 4;
	asm volatile(
		"movl %0, %%ebp \n"
		"movl %1, %%esp \n"
		: : "r" (block->kbp), "r" (block->ksp)
		);

	if (block->signals_flags != 0) {
		isr_stack_t* stack;
		asm volatile(
			"movl (%%ebp), %%ecx \n"
			"movl 8(%%ecx), %0 \n"
			: "=r"(stack)
			);
		if (block->signals_flags & 0x01) {
			block->signals_flags &= 0xFE;
			set_signal(0, stack);
		}
		else if (block->signals_flags & 0x02) {
			block->signals_flags &= 0xFD;
			set_signal(1, stack);
		}
		else if (block->signals_flags & 0x04) {
			block->signals_flags &= 0xFB;
			set_signal(2, stack);
		}
		else if (block->signals_flags & 0x08) {
			block->signals_flags &= 0xF7;
			set_signal(3, stack);
		}
		else if (block->signals_flags & 0x10) {
			block->signals_flags &= 0xEF;
			set_signal(4, stack);
		}
	}
}


/* System Calls*/

/*
int32_t halt
input: uint8_t status = status to use in return value
output: int32_t = failure or success code
purpose: to halt the current process
*/
int32_t halt(uint8_t status) {
	cli();
	/* Close all files associated */
	pcb_t * block = get_pcb(process);
	int32_t i;
	for (i = 2; i < MAXFILES; i++) {
		block->fdarray[i].fops_tbl_pointer->close(i);
	}
	/* Clear out fdarray */
	for (i = 0; i < MAXFILES; i++) {
		block->fdarray[i].fops_tbl_pointer = (void*)null_ops;
		block->fdarray[i].flags = ZERO;
	}

	/* Clear this program's process number */
	process_num[process] = ZERO;

	/* If this process is the top shell, restart */
	if (block->parent_id == -1) {
		clear();
		login_screen();
		terminal_process[process_term] = -1;
		process = -1;
		execute((uint8_t *)"shell");
	}

	if (!strncmp((int8_t*)block->name, (int8_t*)"shell", NAME_SIZE))
		window_exit(process);

	/* Reset process to parent */
	//block = get_pcb(process);
	process = block->parent_id;
	terminal_process[process_term] = process;
	/* Reset page mapping to parent process */
	VtoPmap(onetwentyeightMB, (eightMB + ((process)* fourMB)));

	/* Update tss */
	tss.esp0 = eightMB - (process * eightKB) - 4; //block->parent_ksp;

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

/*
int32_t execute
input: const uint8_t* command = command to execute
output: int32_t shouldn't return from execute, straight from halt
purpose: to execute a program file
*/
int32_t execute(const uint8_t* command) {
	cli();

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
	if (dentry.file_type != 2)
		return -1;
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
	strcpy(block->temp, "");
	block->temp_size = 0;
	block->leftright_idx = 0;
	block->updown_idx = -1;
	block->key_idx = 0;
	block->cycles = 0;
	block->signals_flags = 0;
	block->sig_handler[0] = (int32_t)halt;
	block->sig_handler[1] = (int32_t)halt;
	block->sig_handler[2] = (int32_t)halt;
	block->sig_handler[3] = (int32_t)null_ops;
	block->sig_handler[4] = (int32_t)null_ops;
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

/*
int32_t read
input: int32_t fd = file descriptor for file to read
		void* buf = buffer to write to
		int32_t nbytes = number of bytes to read
output: int32_t = either number of bytes read or -1 for failure
purpose: interface to read from a file
*/
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

/*
int32_t write
input: int32_t fd = file descriptor for file to write to
		const void* buf = buffer to write from
		int32_t nbytes = number of bytes to write
output: int32_t either failure or number of bytes written
purpose: interface to write to a file
*/
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

/*
int32_t open
input: const uint8_t* filename = name of file to open
output: int32_t either success or failure 0, -1
purpose: interface to open a file for use
*/
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
	if (dentry.file_type == RTC_TYPE) {
		block->fdarray[i].fops_tbl_pointer = &rtc_ops;
		block->fdarray[i].file_pos = 512;
	}
	else if (dentry.file_type == DIR_TYPE)
		block->fdarray[i].fops_tbl_pointer = &dir_ops;
	else if (dentry.file_type == FILE_TYPE) {
		block->fdarray[i].fops_tbl_pointer = &file_ops;
		block->fdarray[i].inode = dentry.inode_index;
	}

	block->fdarray[i].fops_tbl_pointer->open(filename);
	return i; // file descriptor is returned
}

/*
int32_t close
input: int32_t fd = file descriptor for file to close
output: int32_t = success or failure 0, -1
purpose: interface for closing a file
*/
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

/*
int32_t getargs
input: uint8_t* buf = buffer to write args to
		int32_t nbytes = max number of bytes to read
output: int32_t = success or failure 0, -1
purpose: to retrieve the argument to this process
*/
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

/*
int32_t vidmap
input: uint8_t** screen_start = pointer to pointer to video memory
output: int32_t = success / failure, 0, -1
purpose: to give user programs a mapping to video memory
*/
int32_t vidmap(uint8_t** screen_start) {
	if (((int32_t)screen_start < onetwentyeightMB) || ((int32_t)screen_start>(onetwentyeightMB + fourMB)))
		return -1;
	uint32_t idx = onetwentyeightMB + fourMB;
	//VtoPpage(idx, VIDEO);
	*screen_start = (uint8_t*)idx;
	return idx;
}

/*
int32_t set_handler
input: int32_t signum = signal identifier
		void* handler_address = address for the handler
output: success or failure, 0, -1
purpose: set the handler for a particular signal
*/
int32_t set_handler(int32_t signum, void* handler_address) {
	if ((signum < 0) || NUM_SIGS <= signum)
		return -1;
	else {
		get_pcb(get_proc())->sig_handler[signum] = (int32_t)handler_address;
		return 0;
	}
}

/*
int32_t sigreturn
input: none
output: int32_t = success or failure, 0, -1
purpose: return from a signal
*/
int32_t sigreturn(void) {
	isr_stack_t* stack1;
	isr_stack_t* stack2;
	asm volatile (
		"movl 16(%%ebp), %0 \n"
		:"=r"(stack1)
		);
	stack2 = (isr_stack_t*)(stack1->useresp + 8);
	stack1->gs = stack2->gs;
	stack1->fs = stack2->fs;
	stack1->es = stack2->es;
	stack1->ds = stack2->ds;
	stack1->edi = stack2->edi;
	stack1->esi = stack2->esi;
	stack1->ebp = stack2->ebp;
	stack1->esp = stack2->esp;
	stack1->ebx = stack2->ebx;
	stack1->edx = stack2->edx;
	stack1->ecx = stack2->ecx;
	stack1->eax = stack2->eax;
	stack1->int_no = stack2->int_no;
	stack1->err_code = stack2->err_code;
	stack1->eip = stack2->eip;
	stack1->cs = stack2->cs;
	stack1->eflags = stack2->eflags;
	stack1->useresp = stack2->useresp;
	stack1->ss = stack2->ss;

	return stack1->eax;
}

/*
int32_t null_ops
input: none
output: success or failure
purpose: to do nothing and return -1
*/
int32_t null_ops(void) {
	return -1;
}

/*

*/
void flag_signal(int32_t signum, int32_t proc) {
	get_pcb(proc)->signals_flags |= (1 << signum);
}

/*

*/
void set_signal(int32_t signum, isr_stack_t* stack) {
	pcb_t* block = get_pcb(get_proc());
	asm volatile(
		"movl %0, %%ecx \n"
		"movl %1, %%edx \n"
		"movl sigret_exec, %%ebx \n"
		"movl %%ebx, -16(%%ecx) \n"
		"movl sigret_exec+4, %%ebx \n"
		"movl %%ebx, -12(%%ecx) \n"
		"movl sigret_exec+8, %%ebx \n"
		"movl %%ebx, -8(%%ecx) \n"
		"movl sigret_exec+12, %%ebx \n"
		"movl %%ebx, -4(%%ecx) \n"
		"subl $16, %%ecx \n"
		"movl %%ecx, set_sig_tmp \n"
		"subl $4, %%ecx \n"
		"movl $19, %%eax \n"
		"set_sig_loop: \n"
		"movl (%%edx), %%ebx \n"
		"movl %%ebx, (%%ecx) \n"
		"subl $4, %%ecx \n"
		"subl $4, %%edx \n"
		"subl $1, %%eax \n"
		"testl %%eax, %%eax \n"
		"ja set_sig_loop \n"
		"movl %2, (%%ecx) \n"
		"subl $4, %%ecx \n"
		"movl set_sig_tmp, %%eax \n"
		"movl %%eax, (%%ecx) \n"
		"pushl %3 \n"
		"pushl %%ecx \n"
		"pushl $0x200 \n"
		"pushl %4 \n"
		"pushl %5 \n"
		"iret \n"
		"set_sig_tmp: .long 0 \n"
		"sigret_exec: \n"
		"movl $10, %%eax \n"
		"int $0x80 \n"
		: : "r"(stack->useresp), "r"(&(stack->ss)), "r"(signum), "r"(stack->ss), "r"(stack->cs), "r"(block->sig_handler[signum])
		/*"%%eax", "%%ebx", "%%ecx", "%%edx"*/
		);
}

/*
pcb_t* get_pcb
input: int32_t proc = process to retrieve pcb for
output: pcb_t* = pointer to pcb of the process
purpose: helper function to get the pcb
*/
pcb_t* get_pcb(int32_t proc) {
	return (pcb_t *)(eightMB - (proc + 1) * eightKB);
}

/*
pcb_t* get_parent_pcb
input: int32_t proc = process to retrieve parent pcb for
output: pcb_t* = pointer to parent pcb of the process
purpose: helper function to get the parent pcb
*/
pcb_t* get_parent_pcb(int32_t proc) {
	return get_pcb(proc)->parent_block;
}

/*
int32_t get_proc
input: none
output: int32_t = currently running process
purpose: helper function to get the current process
*/
int32_t get_proc() {
	return process;
}

/*
int32_t get_term_proc
input: int32_t term = terminal to retrieve process for
output: int32_t = currently running process on specified terminal
purpose: helper function to get the current process on the terminal
*/
int32_t get_term_proc(int32_t term) {
	return terminal_process[term];
}

/*
int32_t get_active
input: none
output: int32_t = currently active terminal
purpose: helper function to get the current terminal
*/
int32_t get_active() {
	return active;
}

/*
int32_t get_proc_term
input: none
output: int32_t = currently running process's terminal
purpose: helper function to get the current process's terminal
*/
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
