

#ifndef _WINDOW_H
#define _WINDOW_H

#include "types.h"

#define BORDER_COLOR 0x74
#define LEFT		0
#define RIGHT		1
#define UP			2
#define DOWN		3


typedef struct window {
	int8_t screen[4000];
	int32_t l;
	int32_t r;
	int32_t t;
	int32_t b;
	int32_t cx;
	int32_t cy;
	uint8_t res[72];
} window_t;


void window_init(int32_t proc);

void window_exit(int32_t proc);

void sizeWindow(int32_t dir);

void moveWindow(int32_t dir);

void shiftWindow(window_t* window);

void updateWindow(window_t* window);

void updateHline(window_t* window, window_t* parent, int32_t y);

void updateVline(window_t* window, window_t* parent, int32_t x);

void borderHline(window_t* window, int32_t y);

void borderVline(window_t* window, int32_t x);

void updateStatus(window_t* window);

window_t* get_window(int32_t proc);

window_t* get_parent_window(int32_t proc);

#endif	/* _WINDOW_H*/
