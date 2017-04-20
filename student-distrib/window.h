#include "types.h"

#define BORDER_COLOR 0x7f
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

window_t* get_window(int32_t proc);

window_t* get_parent_window(int32_t proc);
