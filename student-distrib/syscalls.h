
#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "file.h"

#define onetwentyeightMB	0x8000000
#define eightMB				0x800000
#define fourMB				0x400000
#define eightKB				0x2000
#define FILE_START_POS		0x0000
#define ZERO				0x0000
#define ONE 				0x0001
#define MAXPROCESSES		6
#define MAXFILES			8

/*typedef struct fops_table_t {
	int32_t open(const uint8_t* filename);
	int32_t read(int32_t fd, void* buf, int32_t nbytes);
	int32_t write(int32_t fd, const void* buf, int32_t nbytes);
	int32_t close(int32_t fd);
} fops_tbl_t;*/

typedef struct fd_t {
	uint32_t fops_tbl_pointer[4];
	uint32_t inode;
	uint32_t file_pos;
	uint32_t flags;
} fd_t;

typedef struct pcb_t {
	fd_t fdarray[8];
	uint8_t process;
	uint8_t parent_process;
	pcb_t * parent_block;
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

int32_t null_ops(void)

#endif	/* _SYSCALLS_H*/