#ifndef _FILE_H
#define _FILE_H

#include "types.h"

#define NUM_DENTRY 63
#define NUM_CHARS_NAME 32
#define DENTRY_RESERVED 24
#define NUM_DATA_BLOCKS 1023
#define BOOT_BLOCK_RESERVED 52
#define DATA_BLOCK_SIZE 4096
#define MAX_FILE_NAME 32
#define DATA_BLOCK_SHIFT 12

typedef struct dentry_t {
	int8_t file_name[NUM_CHARS_NAME];
	uint32_t file_type;
	uint32_t inode_index;
	uint8_t reserved[DENTRY_RESERVED];
} dentry_t;

typedef struct inode_t {
	uint32_t length;
	uint32_t data_block_indices[NUM_DATA_BLOCKS];
} inode_t;

typedef struct dblock_t {
	uint8_t data[DATA_BLOCK_SIZE];
} dblock_t;

typedef struct boot_block_t {
	uint32_t num_dentry;
	uint32_t num_inode;
	uint32_t num_dblocks;
	uint8_t reserved[BOOT_BLOCK_RESERVED];
	dentry_t dentries[NUM_DENTRY];
} boot_block_t;

int32_t init_file_sys(uint32_t addr);
int32_t get_inode_length(uint32_t index);

int32_t dir_open();
int32_t dir_close();
int32_t dir_read();
int32_t dir_write();

int32_t file_open();
int32_t file_close();
int32_t file_read();
int32_t file_write();

int32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);







#endif
