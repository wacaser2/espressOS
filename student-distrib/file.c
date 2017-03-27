#include "file.h"
#include "lib.h"

boot_block_t* file_sys;
inode_t* inodes;
dblock_t* dblocks;

/*
* int32_t init_file_sys(uint32_t addr);
*   Inputs: uint32_t addr - the address of the start of the file_sys
*   Return Value: success
*	Function: initialize the pointers to the file_sys
*/
int32_t init_file_sys(uint32_t addr) {
	file_sys = (boot_block_t*)addr;
	inodes = (inode_t*)(file_sys + 1);
	dblocks = (dblock_t*)(inodes + file_sys->num_inode);
	return 0;
}

/*
* int32_t get_inode_length(uint32_t index);
*   Inputs: uint32_t index - index of the inode to return length of
*   Return Value: length of the inode
*	Function: getter function for length of an inode
*/
int32_t get_inode_length(uint32_t index) {
	return (inodes + index)->length;
}

/*
* uint32_t dir_open();
*   Inputs: none
*   Return Value: success / failure
*	Function: open the directory to be accessed
*/
int32_t dir_open() {
	return 0;
}

/*
* uint32_t dir_close();
*   Inputs: none
*   Return Value: success / failure
*	Function: close the directory from access
*/
int32_t dir_close() {
	return  0;
}

/*
* uint32_t dir_read();
*   Inputs: none
*   Return Value: success / failure
*	Function: read from the dir
*/
int32_t dir_read() {
	return 0;
}

/*
* uint32_t dir_write();
*   Inputs: none
*   Return Value: success / failure
*	Function: return error
*/
int32_t dir_write() {
	return -1;
}

/*
* uint32_t file_open();
*   Inputs: none
*   Return Value: success / failure
*	Function: open a file to be read
*/
int32_t file_open() {
	return 0;
}

/*
* int32_t file_close();
*   Inputs: none
*   Return Value: success / failure
*	Function: close a file instance
*/
int32_t file_close() {
	return 0;
}

/*
* uint32_t file_read();
*   Inputs: none
*   Return Value: success / failure
*	Function: read from file
*/
int32_t file_read() {
	return 0;
}

/*
* uint32_t file_write();
*   Inputs: none
*   Return Value: success / failure
*	Function: return error
*/
int32_t file_write() {
	return -1;
}

/*
* uint32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry);
*   Inputs: fname = name of file to read
*			dentry = entry to populate after file found
*   Return Value: success / failure
*	Function: find a file by name and populate a dentry with the values
*/
int32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry) {
	uint32_t i = 0;
	/* filename too long? */
	//if (strlen(fname) > MAX_FILE_NAME)
		//return -1;
	/* set up filename to compare */
	int8_t tmp[MAX_FILE_NAME];
	strncpy(tmp, fname, MAX_FILE_NAME);
	/* find the file with the filename */
	for (i = 0; i < file_sys->num_dentry; i++)
		/* if same name fill in the dentry */
		if (!strncmp(tmp, ((file_sys->dentries)[i]).file_name, MAX_FILE_NAME)) {
			strncpy(dentry->file_name, ((file_sys->dentries)[i]).file_name, MAX_FILE_NAME);
			dentry->file_type = ((file_sys->dentries)[i]).file_type;
			dentry->inode_index = ((file_sys->dentries)[i]).inode_index;
			return 0;
		}
	return -1;
}

/*
* uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
*   Inputs: index = index of file to read
*			dentry = dir entry to fill in
*   Return Value: success / failure
*	Function: find a file and populate a dentry with the values
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
	/* if invalid index */
	if (index >= file_sys->num_dentry)
		return -1;
	/* copy the values */
	strncpy(dentry->file_name, file_sys->dentries[index].file_name, MAX_FILE_NAME);
	dentry->file_type = file_sys->dentries[index].file_type;
	dentry->inode_index = file_sys->dentries[index].inode_index;
	return 0;
}

/*
* int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
*   Inputs: inode = inode index to read from
*			offset = location of file to start reading from
*			buf = location to copy to
*			length = number of bytes to move
*   Return Value: number of bytes read
*	Function: move n bytes of the file starting at offset to the buffer
*/
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



