#include "window.h"
#include "paging.h"
#include "lib.h"
#include "syscalls.h"

/*
void window_init
input: int32_t proc = process to initialize a window for
output: none
purpose: to init a window frame context
*/
void window_init(int32_t proc) {
	if (proc > -1)
		windowPage(proc);
	window_t* window = get_window(proc);
	if (proc == -1 || get_pcb(proc)->parent_id == -1) {
		window->l = 0;
		window->t = 0;
		window->r = NUM_COLS;
		window->b = NUM_ROWS;
	}
	else {
		window_t* prev = get_parent_window(proc);
		window->l = prev->l + 1;
		window->t = prev->t + 1;
		window->r = prev->r - 1;
		window->b = prev->b - 1;
	}
	borderHline(window, window->t - 1);
	borderHline(window, window->b);
	borderVline(window, window->l - 1);
	borderVline(window, window->r);
	updateStatus(window);
	clear();
	update_cursor(window->t, window->l);
}

/*
void window_exit
input: int32_t proc = process to close window
output: none
purpose: close the window to restore the parent window
*/
void window_exit(int32_t proc) {
	updateWindow(get_window(get_parent_pcb(proc)->process_id));
}

/*
void sizeWindow
input: int32_t dir = direction to size the left upper corner from
output: none
purpose: to change the size of the current window
*/
void sizeWindow(int32_t dir) {
	window_t* window = get_window(get_term_proc(get_active()));
	window_t* parent = get_parent_window(get_term_proc(get_active()));
	if (window == parent)
		return;
	switch (dir) {
	case LEFT:
		if (window->l - 1 <= parent->l)
			break;
		window->l--;
		//borderVline(window, window->l - 1);
		shiftWindow(dir);
		fupdate_cursor(window->cy, window->cx - 1);
		break;
	case RIGHT:
		if (window->l + 1 >= window->r)
			break;
		updateVline(window, parent, window->l - 1);
		window->l++;
		//borderVline(window, window->l - 1);
		if (window->cx + 1 < window->r) {
			shiftWindow(dir);
			fupdate_cursor(window->cy, window->cx + 1);
		}
		break;
	case UP:
		if (window->t - 1 <= parent->t)
			break;
		window->t--;
		//borderHline(window, window->t - 1);
		shiftWindow(dir);
		fupdate_cursor(window->cy - 1, window->cx);
		break;
	case DOWN:
		if (window->t + 1 >= window->b)
			break;
		updateHline(window, parent, window->t - 1);
		window->t++;
		//borderHline(window, window->t - 1);
		if (window->cy + 1 < window->b) {
			shiftWindow(dir);
			fupdate_cursor(window->cy + 1, window->cx);
		}
		break;
	default:
		break;
	}
	updateWindow(window);
}

/*
void moveWindow
input: int32_t dir = the direction to move the window
output: none
purpose: to move the current window's position
*/
void moveWindow(int32_t dir) {
	window_t* window = get_window(get_term_proc(get_active()));
	window_t* parent = get_parent_window(get_term_proc(get_active()));
	if (window == parent)
		return;
	switch (dir) {
	case LEFT:
		if (window->l - 1 <= parent->l)
			break;
		updateVline(window, parent, window->r);
		window->l--;
		window->r--;
		//borderVline(window, window->l - 1);
		//borderVline(window, window->r);
		shiftWindow(dir);
		fupdate_cursor(window->cy, window->cx - 1);
		break;
	case RIGHT:
		if (window->r + 1 >= parent->r)
			break;
		updateVline(window, parent, window->l - 1);
		window->l++;
		window->r++;
		//borderVline(window, window->l - 1);
		//borderVline(window, window->r);
		shiftWindow(dir);
		fupdate_cursor(window->cy, window->cx + 1);
		break;
	case UP:
		if (window->t - 1 <= parent->t)
			break;
		updateHline(window, parent, window->b);
		window->t--;
		window->b--;
		//borderHline(window, window->t - 1);
		//borderHline(window, window->b);
		shiftWindow(dir);
		fupdate_cursor(window->cy - 1, window->cx);
		break;
	case DOWN:
		if (window->b + 1 >= parent->b)
			break;
		updateHline(window, parent, window->t - 1);
		window->t++;
		window->b++;
		//borderHline(window, window->t - 1);
		//borderHline(window, window->b);
		shiftWindow(dir);
		fupdate_cursor(window->cy + 1, window->cx);
		break;
	default:
		break;
	}
	updateWindow(window);
}

/*
void shiftWindow
input: int32_t dir
output: none
purpose: to shift the contents of the window in the given direction
*/
void shiftWindow(int32_t dir) {
	window_t* window = get_window(get_term_proc(get_active()));
	window_t* parent = get_parent_window(get_term_proc(get_active()));
	if (window == parent)
		return;
	int32_t i, j;
	switch (dir) {
	case LEFT:
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				if (j == parent->r - 1) {
					fplacec(j, i, ATTRIB, ' ');
				}
				else {
					fplacec(j, i, window->screen[((NUM_COLS*(i)+(j + 1)) << 1) + 1], window->screen[((NUM_COLS*(i)+(j + 1)) << 1)]);
				}
			}
		}
		break;
	case RIGHT:
		for (i = window->t; i < window->b; i++) {
			for (j = window->r - 1; j >= window->l; j--) {
				if (j == parent->l) {
					fplacec(j, i, ATTRIB, ' ');
				}
				else {
					fplacec(j, i, window->screen[((NUM_COLS*(i)+(j - 1)) << 1) + 1], window->screen[((NUM_COLS*(i)+(j - 1)) << 1)]);
				}
			}
		}
		break;
	case UP:
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				if (i == parent->b - 1) {
					fplacec(j, i, ATTRIB, ' ');
				}
				else {
					fplacec(j, i, window->screen[((NUM_COLS*(i + 1) + (j)) << 1) + 1], window->screen[((NUM_COLS*(i + 1) + (j)) << 1)]);
				}
			}
		}
		break;
	case DOWN:
		for (i = window->b - 1; i >= window->t; i--) {
			for (j = window->l; j < window->r; j++) {
				if (i == parent->t) {
					fplacec(j, i, ATTRIB, ' ');
				}
				else {
					fplacec(j, i, window->screen[((NUM_COLS*(i - 1) + (j)) << 1) + 1], window->screen[((NUM_COLS*(i - 1) + (j)) << 1)]);
				}
			}
		}
		break;
	default:
		break;
	}
}

/*
void updateWindow
input: window_t* window = pointer to the window to update
output: none
purpose: to update the window and everything associated
*/
void updateWindow(window_t* window) {
	int32_t i, j;
	for (i = window->t; i < window->b; i++) {
		for (j = window->l; j < window->r; j++) {
			vplacec(j, i, window->screen[((NUM_COLS*(i)+(j)) << 1) + 1], window->screen[((NUM_COLS*(i)+(j)) << 1)]);
		}
	}
	updateStatus(window);
	fupdate_cursor(window->cy, window->cx);
}

/*
void updateHline
input: window_t* window = prev window occupying this line, parent = window to update this line to
		int32_t y = the y position of the line to update
output: none
purpose: to update the line to a different window
*/
void updateHline(window_t* window, window_t* parent, int32_t y) {
	int32_t i;
	for (i = window->l - 1; i <= window->r; i++) {
		vplacec(i, y, parent->screen[((NUM_COLS*(y)+(i)) << 1) + 1], parent->screen[((NUM_COLS*(y)+(i)) << 1)]);
	}
}

/*
void updateVline
input: window_t* window = prev window occupying this line, parent = window to update this line to
		int32_t x = the x position of the line to update
output: none
purpose: to update the line to a different window
*/
void updateVline(window_t* window, window_t* parent, int32_t x) {
	int32_t i;
	for (i = window->t - 1; i <= window->b; i++) {
		vplacec(x, i, parent->screen[((NUM_COLS*(i)+(x)) << 1) + 1], parent->screen[((NUM_COLS*(i)+(x)) << 1)]);
	}
}

/*
void borderHline
input: window_t* window = window to border
		int32_t y = y position of line to border
output: none
purpose: add a border line to the window
*/
void borderHline(window_t* window, int32_t y) {
	int32_t i;
	for (i = window->l - 1; i <= window->r; i++) {
		vplacec(i, y, BORDER_COLOR, '-');
	}
}

/*
void borderVline
input: window_t* window = window to border
		int32_t x = x position of line to border
output: none
purpose: add a border line to the window
*/
void borderVline(window_t* window, int32_t x) {
	int32_t i;
	for (i = window->t - 1; i <= window->b; i++) {
		vplacec(x, i, BORDER_COLOR, '|');
	}
}

/*
void updateStatus
input: window_t* window = window to border
output: none
purpose: to add or refresh the border around a window
*/
void updateStatus(window_t* window) {
	borderHline(window, window->t - 1);
	borderHline(window, window->b);
	borderVline(window, window->l - 1);
	borderVline(window, window->r);
	vplacec(window->l, window->t - 1, BORDER_COLOR, 'S');
	vplacec(window->l + 1, window->t - 1, BORDER_COLOR, 'h');
	vplacec(window->l + 2, window->t - 1, BORDER_COLOR, 'e');
	vplacec(window->l + 3, window->t - 1, BORDER_COLOR, 'l');
	vplacec(window->l + 4, window->t - 1, BORDER_COLOR, 'l');
	vplacec(window->l + 5, window->t - 1, BORDER_COLOR, '-');
	vplacec(window->r - 1, window->t - 1, BORDER_COLOR, '-');
	vplacec(window->r, window->t, BORDER_COLOR, '|');
	vplacec(window->r, window->t - 1, BORDER_COLOR, 'X');
}

/*
window_t* get_window
input: int32_t proc = process to get the window to
output: window_t* = the window of the proc
purpose: getter function for the window of a proc
*/
window_t* get_window(int32_t proc) {
	if (proc == -1)
		return (window_t*)VIDEO;
	return (window_t*)(fourMB*(2 + MAXPROCESSES) + ((get_pcb(proc)->window_id) << 12));
}

/*
window_t* get_parent_window
input: int32_t proc = process to get the parent window to
output: window_t* = the parent window of the proc
purpose: getter function for the parent window of a proc
*/
window_t* get_parent_window(int32_t proc) {
	if (proc == -1)
		return (window_t*)VIDEO;
	return (window_t*)(fourMB*(2 + MAXPROCESSES) + ((get_parent_pcb(get_pcb(proc)->window_id)->window_id) << 12));
}

