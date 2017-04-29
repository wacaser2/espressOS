#include "window.h"
#include "paging.h"
#include "lib.h"
#include "syscalls.h"

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
		window_t* prev = get_window(get_parent_pcb(proc)->process_id);
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

void window_exit(int32_t proc) {
	restore(proc);
}

void sizeWindow(int32_t dir) {
	window_t* window = get_window(get_proc());
	window_t* parent = get_parent_window(get_proc());
	if (get_proc() < 1)
		return;
	switch (dir) {
	case LEFT:
		if (window->l - 1 <= parent->l)
			break;
		window->l--;
		borderVline(window, window->l - 1);
		update_cursor(window->cy, window->cx - 1);
		break;
	case RIGHT:
		if (window->cx + 1 >= window->r)
			break;
		updateVline(window, parent, window->l - 1);
		window->l++;
		borderVline(window, window->l - 1);
		update_cursor(window->cy, window->cx + 1);
		break;
	case UP:
		if (window->t - 1 <= parent->t)
			break;
		window->t--;
		borderHline(window, window->t - 1);
		update_cursor(window->cy - 1, window->cx);
		break;
	case DOWN:
		if (window->cy + 1 >= window->b)
			break;
		updateHline(window, parent, window->t - 1);
		window->t++;
		borderHline(window, window->t - 1);
		update_cursor(window->cy + 1, window->cx);
		break;
	default:
		break;
	}
	updateWindow(window);
}

void moveWindow(int32_t dir) {
	window_t* window = get_window(get_proc());
	window_t* parent = get_parent_window(get_proc());
	if (get_proc() < 1)
		return;
	switch (dir) {
	case LEFT:
		if (window->l - 1 <= parent->l)
			break;
		updateVline(window, parent, window->r);
		window->l--;
		window->r--;
		borderVline(window, window->l - 1);
		borderVline(window, window->r);
		update_cursor(window->cy, window->cx - 1);
		break;
	case RIGHT:
		if (window->r + 1 >= parent->r)
			break;
		updateVline(window, parent, window->l - 1);
		window->l++;
		window->r++;
		borderVline(window, window->l - 1);
		borderVline(window, window->r);
		update_cursor(window->cy, window->cx + 1);
		break;
	case UP:
		if (window->t - 1 <= parent->t)
			break;
		updateHline(window, parent, window->b);
		window->t--;
		window->b--;
		borderHline(window, window->t - 1);
		borderHline(window, window->b);
		update_cursor(window->cy - 1, window->cx);
		break;
	case DOWN:
		if (window->b + 1 >= parent->b)
			break;
		updateHline(window, parent, window->t - 1);
		window->t++;
		window->b++;
		borderHline(window, window->t - 1);
		borderHline(window, window->b);
		update_cursor(window->cy + 1, window->cx);
		break;
	default:
		break;
	}
	updateWindow(window);
}

void updateWindow(window_t* window) {
	int32_t i, j;
	for (i = window->t; i < window->b; i++) {
		for (j = window->l; j < window->r; j++) {
			placec(j, i, window->screen[((NUM_COLS*(i) + (j)) << 1) + 1], window->screen[((NUM_COLS*(i) + (j)) << 1)]);
		}
	}
	borderHline(window, window->t - 1);
	borderHline(window, window->b);
	borderVline(window, window->l - 1);
	borderVline(window, window->r);
	updateStatus(window);
	update_cursor(window->cy, window->cx);
}

void updateHline(window_t* window, window_t* parent, int32_t y) {
	int32_t i;
	for (i = window->l - 1; i <= window->r; i++) {
		placec(i, y, parent->screen[((NUM_COLS*(y) + (i)) << 1) + 1], parent->screen[((NUM_COLS*(y) + (i)) << 1)]);
	}
}

void updateVline(window_t* window, window_t* parent, int32_t x) {
	int32_t i;
	for (i = window->t - 1; i <= window->b; i++) {
		placec(x, i, parent->screen[((NUM_COLS*(i) + (x)) << 1) + 1], parent->screen[((NUM_COLS*(i) + (x)) << 1)]);
	}
}

void borderHline(window_t* window, int32_t y) {
	int32_t i;
	for (i = window->l - 1; i <= window->r; i++) {
		placec(i, y, BORDER_COLOR, ' ');
	}
}

void borderVline(window_t* window, int32_t x) {
	int32_t i;
	for (i = window->t - 1; i <= window->b; i++) {
		placec(x, i, BORDER_COLOR, ' ');
	}
}

void updateStatus(window_t* window) {
	placec(window->l, window->t - 1, BORDER_COLOR, 'S');
	placec(window->l + 1, window->t - 1, BORDER_COLOR, 'h');
	placec(window->l + 2, window->t - 1, BORDER_COLOR, 'e');
	placec(window->l + 3, window->t - 1, BORDER_COLOR, 'l');
	placec(window->l + 4, window->t - 1, BORDER_COLOR, 'l');
	placec(window->l + 5, window->t - 1, BORDER_COLOR, ' ');
	placec(window->r - 1, window->t - 1, BORDER_COLOR, ' ');
	placec(window->r, window->t, BORDER_COLOR, ' ');
	placec(window->r, window->t - 1, BORDER_COLOR, 'X');
}

window_t* get_window(int32_t proc) {
	if (proc == -1)
		return (window_t*)VIDEO;
	return (window_t*)(VIDEO + ((1 + get_pcb(proc)->window_id) << 12));
}

window_t* get_parent_window(int32_t proc) {
	if (proc == -1)
		return (window_t*)VIDEO;
	return (window_t*)(VIDEO + ((1 + get_parent_pcb(proc)->window_id) << 12));
}

