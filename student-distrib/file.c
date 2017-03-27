#include "file.h"
#include "lib.h"

boot_block_t* file_sys;
inode_t* inodes;
dblock_t* dblocks;

int32_t init_file_sys(uint32_t addr) {
	file_sys = (boot_block_t*)addr;
	inodes = (inode_t*)(file_sys + 1);
	dblocks = (dblock_t*)(inodes + file_sys->num_inode);
	return 0;
}

int32_t get_inode_length(uint32_t index) {
	return (inodes + index)->length;
}

int32_t dir_open() {
	return 0;
}
int32_t dir_close() {
	return  0;
}
int32_t dir_read() {
	return 0;
}
int32_t dir_write() {
	return -1;
}

int32_t file_open() {
	return 0;
}
int32_t file_close() {
	return 0;
}
int32_t file_read() {
	return 0;
}
int32_t file_write() {
	return -1;
}

int32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry) {
	uint32_t len = strlen(fname), i = 0;
	if (len > MAX_FILE_NAME)
		return -1;
	int8_t tmp[MAX_FILE_NAME];
	strncpy(tmp, fname, MAX_FILE_NAME);
	for (i = 0; i < file_sys->num_dentry; i++)
		if (!strncmp(tmp, ((file_sys->dentries)[i]).file_name, MAX_FILE_NAME)) {
			strncpy(dentry->file_name, ((file_sys->dentries)[i]).file_name, MAX_FILE_NAME);
			dentry->file_type = ((file_sys->dentries)[i]).file_type;
			dentry->inode_index = ((file_sys->dentries)[i]).inode_index;
			return 0;
		}
	return -1;
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
	if (index >= file_sys->num_dentry)
		return -1;
	strncpy(dentry->file_name, file_sys->dentries[index].file_name, MAX_FILE_NAME);
	dentry->file_type = file_sys->dentries[index].file_type;
	dentry->inode_index = file_sys->dentries[index].inode_index;
	return 0;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
	if (inode >= file_sys->num_inode)
		return -1;
	inode_t* in = inodes + inode;
	if (offset > in->length)
		return 0;
	uint32_t rem = length, curr = offset, loc, dbi, cpylen;
	if (offset + length > in->length)
		length = rem = in->length - offset;
	while (rem != 0) {
		dbi = in->data_block_indices[(curr >> DATA_BLOCK_SHIFT)];
		if (dbi >= file_sys->num_dblocks)
			return -1;
		loc = curr&(DATA_BLOCK_SIZE - 1);
		cpylen = DATA_BLOCK_SIZE - loc;
		cpylen = (cpylen > rem) ? rem : cpylen;
		memcpy((buf + curr - offset), (((dblocks + dbi)->data) + loc), cpylen);
		curr += cpylen;
		rem -= cpylen;
	}
	return length;
}



