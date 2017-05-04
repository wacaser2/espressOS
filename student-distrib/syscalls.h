
#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "file.h"
#include "keyboard.h"
#include "rtc.h"
#include "mouse.h"

#define onetwentyeightMB	0x8000000
#define eightMB				0x800000
#define fourMB				0x400000
#define eightKB				0x2000
#define FILE_START_POS		0x0000
#define ZERO				0x0000
#define ONE 				0x0001
#define MAXPROCESSES		12
#define MAXFILES			8
#define RTC_TYPE			0
#define DIR_TYPE			1
#define FILE_TYPE			2
#define MOUSE_TYPE		3
#define NAME_SIZE			32
#define ARGBUF_SIZE			128
#define INT_BYTES			4
#define PROG_START			0x08048000
#define PROG_STACK_START	0x83FFFFC
#define EXCEPTION_CODE		256

typedef struct fops_table_t {
	int32_t(*open)(const uint8_t* filename);						//function to open this file
	int32_t(*read)(int32_t fd, void* buf, int32_t nbytes);			//function to read from file
	int32_t(*write)(int32_t fd, const void* buf, int32_t nbytes);	//function to write to file
	int32_t(*close)(int32_t fd);									//function to close this file
} fops_tbl_t;

typedef struct fd_t {
	fops_tbl_t * fops_tbl_pointer;		//table of functions to call for this file
	uint32_t inode;						//inode associated with this file
	uint32_t file_pos;					//position in the current file
	uint32_t flags;						//flag variable(currently only present flag)
} fd_t;

typedef struct pcb_t {
	fd_t fdarray[MAXFILES];			//array of file descriptor entries
	uint8_t name[NAME_SIZE];		//name of process
	uint8_t args[ARGBUF_SIZE];		//buffer containing the argument to this process
	int8_t command[ARGBUF_SIZE];	//buffer for the command issued to this process
	int8_t temp[ARGBUF_SIZE];		//temporary buffer for text input
	int32_t temp_size;				//size of input on temp buffer
	int32_t leftright_idx;			//index of cursor in current input to process
	int32_t updown_idx;				//index in command history for this process
	int32_t key_idx;				//index of the farthest key in the input
	int32_t enter_flag;				//flag for enter being pressed while in this process
	int32_t cycles;					//cycles left in time quantum
	int32_t window_id;				//id number of window for this process
	int32_t process_id;				//id number of this process
	int32_t parent_id;				//id number of parent process
	uint32_t kbp;					//kbp to return to after context switch
	uint32_t ksp;					//ksp to return to after context switch
	uint32_t parent_kbp;			//parent kbp to return to after halt
	uint32_t parent_ksp;			//parent ksp to return to after halt
	struct pcb_t * parent_block;	//pcb of parent process
} pcb_t;

void switch_active(int32_t term);

void switch_process(int32_t term);

int32_t halt(uint8_t status);

int32_t execute(const uint8_t* command);

int32_t read(int32_t fd, void* buf, int32_t nbytes);

int32_t write(int32_t fd, const void* buf, int32_t nbytes);

int32_t open(const uint8_t* filename);

int32_t close(int32_t fd);

int32_t getargs(uint8_t* buf, int32_t nbytes);

int32_t vidmap(uint8_t** screen_start);

int32_t set_handler(int32_t signum, void* handler_address);

int32_t sigreturn(void);

int32_t null_ops(void);

pcb_t* get_pcb(int32_t proc);

pcb_t* get_parent_pcb(int32_t proc);

int32_t get_proc();

int32_t get_term_proc(int32_t term);

int32_t get_active();

int32_t get_proc_term();

void implicit_proc();

#endif	/* _SYSCALLS_H*/
