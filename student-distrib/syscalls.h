
#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "file.h"
#include "keyboard.h"
#include "rtc.h"

#define onetwentyeightMB	0x8000000
#define eightMB				0x800000
#define fourMB				0x400000
#define eightKB				0x2000
#define FILE_START_POS		0x0000
#define ZERO				0x0000
#define ONE 				0x0001
#define MAXPROCESSES		6
#define MAXFILES			8
#define RTC_TYPE			0
#define DIR_TYPE			1
#define FILE_TYPE			2
#define NAME_SIZE			32
#define ARGBUF_SIZE			128
#define INT_BYTES			4
#define PROG_START			0x08048000
#define PROG_STACK_START	0x83FFFFC
#define EXCEPTION_CODE		256

typedef struct fops_table_t {
	int32_t(*open)(const uint8_t* filename);
	int32_t(*read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t(*write)(int32_t fd, const void* buf, int32_t nbytes);
	int32_t(*close)(int32_t fd);
} fops_tbl_t;

typedef struct fd_t {
	fops_tbl_t * fops_tbl_pointer;
	uint32_t inode;
	uint32_t file_pos;
	uint32_t flags;
} fd_t;

typedef struct pcb_t {
	fd_t fdarray[MAXFILES];
	uint8_t name[NAME_SIZE];
	uint8_t args[ARGBUF_SIZE];
	int32_t window_id;
	uint8_t process_id;
	uint32_t parent_kbp;
	uint32_t parent_ksp;
	struct pcb_t * parent_block;
} pcb_t;

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

void implicit_proc();

#endif	/* _SYSCALLS_H*/
