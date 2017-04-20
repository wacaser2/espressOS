#include "window.h"
#include "paging.h"
#include "lib.h"
#include "syscalls.h"

void window_init(int32_t proc) {
	if (proc > -1)
		windowPage(proc);
	window_t* window = get_window(proc);
	if (proc == 0 || proc == -1) {
		window->l = 0;
		window->t = 0;
		window->r = NUM_COLS;
		window->b = NUM_ROWS;
	}
	else {
		window_t* prev = get_window(proc - 1);
		window->l = prev->l + 1;
		window->t = prev->t + 1;
		window->r = prev->r - 1;
		window->b = prev->b - 1;
	}
	int32_t i;
	for (i = window->t - 1; i <= window->b; i++) {
		placec(window->l - 1, i, BORDER_COLOR, ' ');
		placec(window->r, i, BORDER_COLOR, ' ');
	}
	for (i = window->l - 1; i <= window->r; i++) {
		placec(i, window->t - 1, BORDER_COLOR, ' ');
		placec(i, window->b, BORDER_COLOR, ' ');
	}
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
	int32_t i, j;
	switch (dir) {
	case LEFT:
		if (window->l - 1 <= parent->l)
			break;
		window->l--;
		for (i = window->t - 1; i <= window->b; i++) {
			placec(window->l - 1, i, BORDER_COLOR, ' ');
		}
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				placec(j, i, window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1) + 1], window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1)]);
			}
		}
		update_cursor(window->cy, window->cx - 1);
		break;
	case RIGHT:
		if (window->cx + 1 >= window->r)
			break;
		for (i = window->t - 1; i <= window->b; i++) {
			placec(window->l - 1, i, parent->screen[((NUM_COLS*(i - parent->t) + (window->l - 1 - parent->l)) << 1) + 1], parent->screen[((NUM_COLS*(i - parent->t) + (window->l - 1 - parent->l)) << 1)]);
		}
		window->l++;
		for (i = window->t - 1; i <= window->b; i++) {
			placec(window->l - 1, i, BORDER_COLOR, ' ');
		}
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				placec(j, i, window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1) + 1], window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1)]);
			}
		}
		update_cursor(window->cy, window->cx + 1);
		break;
	case UP:
		if (window->t - 1 <= parent->t)
			break;
		window->t--;
		for (i = window->l - 1; i <= window->r; i++) {
			placec(i, window->t - 1, BORDER_COLOR, ' ');
		}
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				placec(j, i, window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1) + 1], window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1)]);
			}
		}
		update_cursor(window->cy - 1, window->cx);
		break;
	case DOWN:
		if (window->cy + 1 >= window->b)
			break;
		for (i = window->l - 1; i <= window->r; i++) {
			placec(i, window->t - 1, parent->screen[((NUM_COLS*(window->t - 1 - parent->t) + (i - parent->l)) << 1) + 1], parent->screen[((NUM_COLS*(window->t - 1 - parent->t) + (i - parent->l)) << 1)]);
		}
		window->t++;
		for (i = window->l - 1; i <= window->r; i++) {
			placec(i, window->t - 1, BORDER_COLOR, ' ');
		}
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				placec(j, i, window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1) + 1], window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1)]);
			}
		}
		update_cursor(window->cy + 1, window->cx);
		break;
	default:
		break;
	}
}

void moveWindow(int32_t dir) {
	window_t* window = get_window(get_proc());
	window_t* parent = get_parent_window(get_proc());
	if (get_proc() < 1)
		return;
	int32_t i, j;
	switch (dir) {
	case LEFT:
		if (window->l - 1 <= parent->l)
			break;
		for (i = window->t - 1; i <= window->b; i++) {
			placec(window->r, i, parent->screen[((NUM_COLS*(i - parent->t) + (window->r - parent->l)) << 1) + 1], parent->screen[((NUM_COLS*(i - parent->t) + (window->r - parent->l)) << 1)]);
		}
		window->l--;
		window->r--;
		for (i = window->t - 1; i <= window->b; i++) {
			placec(window->l - 1, i, BORDER_COLOR, ' ');
			placec(window->r, i, BORDER_COLOR, ' ');
		}
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				placec(j, i, window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1) + 1], window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1)]);
			}
		}
		update_cursor(window->cy, window->cx - 1);
		break;
	case RIGHT:
		if (window->r + 1 >= parent->r)
			break;
		for (i = window->t - 1; i <= window->b; i++) {
			placec(window->l - 1, i, parent->screen[((NUM_COLS*(i - parent->t) + (window->l - 1 - parent->l)) << 1) + 1], parent->screen[((NUM_COLS*(i - parent->t) + (window->l - 1 - parent->l)) << 1)]);
		}
		window->l++;
		window->r++;
		for (i = window->t - 1; i <= window->b; i++) {
			placec(window->l - 1, i, BORDER_COLOR, ' ');
			placec(window->r, i, BORDER_COLOR, ' ');
		}
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				placec(j, i, window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1) + 1], window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1)]);
			}
		}
		update_cursor(window->cy, window->cx + 1);
		break;
	case UP:
		if (window->t - 1 <= parent->t)
			break;
		for (i = window->l - 1; i <= window->r; i++) {
			placec(i, window->b, parent->screen[((NUM_COLS*(window->b - parent->t) + (i - parent->l)) << 1) + 1], parent->screen[((NUM_COLS*(window->b - parent->t) + (i - parent->l)) << 1)]);
		}
		window->t--;
		window->b--;
		for (i = window->l - 1; i <= window->r; i++) {
			placec(i, window->t - 1, BORDER_COLOR, ' ');
			placec(i, window->b, BORDER_COLOR, ' ');
		}
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				placec(j, i, window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1) + 1], window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1)]);
			}
		}
		update_cursor(window->cy - 1, window->cx);
		break;
	case DOWN:
		if (window->b + 1 >= parent->b)
			break;
		for (i = window->l - 1; i <= window->r; i++) {
			placec(i, window->t - 1, parent->screen[((NUM_COLS*(window->t - 1 - parent->t) + (i - parent->l)) << 1) + 1], parent->screen[((NUM_COLS*(window->t - 1 - parent->t) + (i - parent->l)) << 1)]);
		}
		window->t++;
		window->b++;
		for (i = window->l - 1; i <= window->r; i++) {
			placec(i, window->t - 1, BORDER_COLOR, ' ');
			placec(i, window->b, BORDER_COLOR, ' ');
		}
		for (i = window->t; i < window->b; i++) {
			for (j = window->l; j < window->r; j++) {
				placec(j, i, window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1) + 1], window->screen[((NUM_COLS*(i - window->t) + (j - window->l)) << 1)]);
			}
		}
		update_cursor(window->cy + 1, window->cx);
		break;
	default:
		break;
	}
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

