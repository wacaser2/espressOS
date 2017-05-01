

#ifndef _WINDOW_H
#define _WINDOW_H

#include "types.h"

#define BORDER_COLOR 0x74
#define LEFT		0
#define RIGHT		1
#define UP			2
#define DOWN		3


typedef struct window {
	int8_t screen[4000];//text mode memory
	int32_t l;			//left edge position
	int32_t r;			//right edge position
	int32_t t;			//top edge position
	int32_t b;			//bottom edge position
	int32_t cx;			//cursor x position
	int32_t cy;			//cursor y position
	uint8_t res[72];	//reserved
} window_t;


void window_init(int32_t proc);

void window_exit(int32_t proc);

void sizeWindow(int32_t dir);

void moveWindow(int32_t dir);

void shiftWindow(int32_t dir);

void updateWindow(window_t* window);

void updateHline(window_t* window, window_t* parent, int32_t y);

void updateVline(window_t* window, window_t* parent, int32_t x);

void borderHline(window_t* window, int32_t y);

void borderVline(window_t* window, int32_t x);

void updateStatus(window_t* window);

window_t* get_window(int32_t proc);

window_t* get_parent_window(int32_t proc);

#endif	/* _WINDOW_H*/
