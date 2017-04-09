
#include "lib.h"
#include "x86_desc.h"
#include "syscalls.h"

uint8_t process_num[6] = {0, 0, 0, 0, 0, 0}; 

uint32_t stdin_ops[4] = {(uint32_t)null_ops, (uint32_t)null_ops, (uint32_t)terminal_read, (uint32_t)null_ops, (uint32_t)null_ops};
uint32_t stdout_ops[4] = {(uint32_t)null_ops, (uint32_t)null_ops, (uint32_t)terminal_write, (uint32_t)null_ops};
uint32_t rtc_ops[4] = {(uint32_t)rtc_open, (uint32_t)rtc_read, (uint32_t)rtc_write, (uint32_t)rtc_close};
uint32_t file_ops[4] = {(uint32_t)file_open, (uint32_t)file_read, (uint32_t)file_write, (uint32_t)file_close};
uint32_t dir_ops[4] = {(uint32_t)dir_open, (uint32_t)dir_read, (uint32_t)dir_write, (uint32_t)dir_close};
uint32_t default_ops[4] = {(uint32_t)null_ops, (uint32_t)null_ops, (uint32_t)null_ops, (uint32_t)null_ops, (uint32_t)null_ops};

/* System Calls*/

int32_t halt(uint8_t status){
	return 0;
}

int32_t execute(const uint8_t* command){
	int start = 0;
	while(command[start] == ' '){
		start++;
	}
	int end = start;
	while(command[end] != ' '){
		end++;
	}
	uint8_t name[32];
	uint8_t restarg[128];
	int i, j;
	for(i = 0, j = start; i < (end - start), j < end; i++, j++)
	{
		name[i] = command[j];
	}
	name[i] = '\0';
	end++;
	start = end;
	while(command[end] != '\0'){
		end++;
	}
	for(i = 0, j = start; i < (end - start), j < end; i++, j++)
	{
		restarg[i] = command[j];;
	}
	restarg[i] = '\0';


	dentry_t * dentry;
	if(read_dentry_by_name(name, dentry) == -1){
		return -1;
	}

	uint8_t buffer[4];
	uint8_t magic_number[4] = {0x7f, 0x45, 0x4c, 0x46};		// DEL, E, L, F
	read_data(dentry->inode_index, 0, buffer, 4);
	if(strcmp(buffer, magic_number) != 0){					// if you don't get the magic numbers in the first four bytes
		return -1
	}

	uint32_t * entry_point;
	read_data(dentry->inode_index, 24, entry_point, 4);

	int process = -1;	
	for(i = 0; i < MAXPROCESSES; i++){
		if(process_num[i] == 0){
			process_num[i] = 1;
			process = i;
			break;
		}
	}
	if(process < 0){
		return -1;
	}

	VtoPmap(onetwentyeightMB, (eightMB + (process * fourMB)));

	read_data(dentry->inode_index, 0, 0x8048000, 8192000);

	pcb_t * block = (pcb_t *) (eightMB - (process + 1) * eightKB);	// top of the 8KB stack
	block->process = process;

	if(process == 0){						// first process, so set parent to itself
		block->parent_block->process = process;
		
	}
	else{

	}

	strcpy(block->buf, restarg);

	/* STDIN */
	block->fdarray[ZERO].fops_tbl_pointer = stdin_ops;		
	block->fdarray[ZERO].inode = -1;					// or 0?
	block->fdarray[ZERO].file_pos = FILE_START_POS;
	block->fdarray[ZERO].flags = ONE;					// in use
	/* STDOUT */
	block->fdarray[ONE].fops_tbl_pointer = stdout_ops;
	block->fdarray[ONE].inode = -1;
	block->fdarray[ONE].file_pos = FILE_START_POS;
	block->fdarray[ONE].flags = ONE;

	/* Intialize the remaining files to the default values */
	for(i = 2; i < MAXFILES; i++){		
		block->fdarray[i].fops_tbl_pointer = default_ops;		
		block->fdarray[i].inode = -1;
		block->fdarray[i].file_pos = FILE_START_POS;
		block->fdarray[i].flags = ZERO;					// not in use
	}

	tss.ss0 = KERNEL_DS;
	tss.esp0 = eightMB - (process * eightKB) - 4;

	asm volatile (
		"movw ; \n \t"
		: /* no outputs */
		: "r"(page_directory)		// input operands
		: "eax"						// clobbers
		);


	return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){
	return 0;
}

int32_t open(const uint8_t* filename){
	return 0;
}

int32_t close(int32_t fd){
	return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes){
	return 0;
}

int32_t vidmap(uint8_t** screen_start){
	return 0;
}

int32_t set_handler(int32_t signum, void* handler_address){
	return 0;
}

int32_t sigreturn(void){
	return 0;
}

int32_t null_ops(void){
	return -1;
}