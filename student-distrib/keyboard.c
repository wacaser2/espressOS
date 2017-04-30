#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "syscalls.h"

/* implemeting keyboard function variables */
volatile int enter_flag = 0;
volatile int ctrl_flag = 0;
volatile int shift_flag = 0;
volatile int capslock_flag = 0;
volatile int8_t key_buf[KEY_BUF_SIZE];
volatile int key_idx = 0;
volatile unsigned char temp_char;

/* buffer command history variables */
volatile int8_t buf_hist[MAX_COMMANDS][KEY_BUF_SIZE];
volatile unsigned char buf_hist_cmd_size[MAX_COMMANDS];
volatile int updown_idx = 0;
volatile int write_idx = 0; // while typing in a cmd store cmd idx
volatile int buf_size = 1;

/* current prompt info */
volatile int8_t temp[KEY_BUF_SIZE];
volatile int temp_size = 0;

/* variables for left and right function */
volatile uint32_t  leftright_idx = 0;

/* keyboard array*/
static unsigned char key[KEY_BUF_SIZE_ACTUAL] =
{
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
	0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
	0,	/* Alt */
  ' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
  '-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
  '+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};


/* keyboard array with shift pressed */
static unsigned char shift_key[KEY_BUF_SIZE_ACTUAL] =
{
	0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', '\b', /* Backspace */
  '\t',     /* Tab */
  'Q', 'W', 'E', 'R', /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
	0,      /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', /* 39 */
 '"', '~',   0,    /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
  'M', '<', '>', '?',   0,        /* Right shift */
  '*',
	0,  /* Alt */
  ' ',  /* Space bar */
	0,  /* Caps lock */
	0,  /* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,  /* < ... F10 */
	0,  /* 69 - Num lock*/
	0,  /* Scroll Lock */
	0,  /* Home key */
	0,  /* Up Arrow */
	0,  /* Page Up */
  '-',
	0,  /* Left Arrow */
	0,
	0,  /* Right Arrow */
  '+',
	0,  /* 79 - End key*/
	0,  /* Down Arrow */
	0,  /* Page Down */
	0,  /* Insert Key */
	0,  /* Delete Key */
	0,   0,   0,
	0,  /* F11 Key */
	0,  /* F12 Key */
	0,  /* All other keys are undefined */
};


/*
* void keyboard_init()
*   Inputs: none
*   Return Value: void
*	Function: inits the keyboard and enables the interrupts for it
*/
void keyboard_init()
{
	enable_irq(KEYBOARD_IRQ);	// enable keyboard interrupt
}

/*
* void keyboard_handler()
*   Inputs: none
*   Return Value: void
*	Function: Handles a keyboard interrupt by outputting the char
*/
void keyboard_handler()
{
	uint8_t scancode = 0;
	int i; // looping variable

	/* Read from the keyboard's data buffer */
	scancode = inb((int)KEYBOARD_PORT);

	send_eoi(KEYBOARD_IRQ);	//allow more interrupts to queue for the keyboard
	/* If the top bit of the byte we read from the keyboard is
	*  set, that means that a key has just been released */
	if (scancode & 0x80)
	{
		/* You can use this one to see if the user released the
		 *  shift, alt, or control keys... */
		if (scancode == LEFT_CTRL_REL)
			ctrl_flag = 0; /* set control flag to zero when key is released */

		if (scancode == LEFT_SHIFT_REL || scancode == RIGHT_SHIFT_REL)
			shift_flag = 0; /* set shift flag to zero when key is released */
	}
	else
	{
		if (scancode == BACKSPACE) // case for backspace
		{
			if (key_idx > 0 && leftright_idx > 0)
			{
				backspace_put(0) ; // move cursor
				--key_idx;

				/* to display onto video */
				i = leftright_idx;
				while(i<key_idx+1){
					key_buf[i-1] = key_buf[i]; 
					putc(key_buf[i]); 
					++i;
				}
				putc(NULL_KEY);
				key_buf[i] = NULL_KEY;

				/* move cursor back to correct place */
				for(i=0; i<(key_idx+1)-(leftright_idx-1); ++i)
					move_cursor_left();

				/* case where we're changing prompt after hitting RETURN */
				if (updown_idx == write_idx){
					i = leftright_idx;
					for(i=0; i<key_idx; ++i)
						temp[i] = key_buf[i];
					temp[i] = NULL_KEY;
					--temp_size;
				}
				--leftright_idx;
			}
		}
		else if (scancode == LEFT_KEY)
		{
			//printf("%d", leftright_idx);
			if(leftright_idx != 0){
				--leftright_idx;
				move_cursor_left();
			}

		}
		else if (scancode == RIGHT_KEY)
		{
			if(leftright_idx != key_idx){
				++leftright_idx;
				move_cursor_right();
			}
		}
		else if (!(ctrl_flag || shift_flag) && (scancode == UP_KEY || scancode == DOWN_KEY))
		{
			/* move to prev command and add required backspaces */
			if(key_idx != leftright_idx){
				i=0;
				while(i<key_idx-leftright_idx){
					move_cursor_right();
					++i;
				}
			}
			i=key_idx;
			while (i > 0) {
				backspace_put(0);
				i--;
			}

			/* change the updown_idx */
			if (scancode == UP_KEY) {
				if (buf_size != MAX_COMMANDS && updown_idx == 0) {}
				else if ((buf_size + updown_idx - 1) % buf_size != write_idx) updown_idx = (buf_size + updown_idx - 1) % buf_size;
				else updown_idx = (write_idx + 1) % MAX_COMMANDS;
			}
			else if (scancode == DOWN_KEY) {
				if (updown_idx != write_idx) updown_idx = (buf_size + updown_idx + 1) % buf_size;
				else updown_idx = write_idx;
			}

			/* change key_idx to updown value */
			if (updown_idx == write_idx) key_idx = temp_size;
			else key_idx = buf_hist_cmd_size[updown_idx];

			/* left and right value updated when up/down pressed */
			leftright_idx = key_idx;

			/* print the command at the updown_idx */
			for (i = 0; i < key_idx; ++i) {
				if (updown_idx == write_idx) key_buf[i] = temp[i];
				else key_buf[i] = buf_hist[updown_idx][i];
				putc(key_buf[i]);
			}
		}
		else if (scancode == ENTER || key[scancode] == CARRIAGE_RETURN)
		{
			/* works only when command is entered */
			if (key_idx != 0) {
				/* case where we are at the same place where we're writing */
				if (updown_idx == write_idx) {
					for (i = 0; i < temp_size; ++i)
						buf_hist[write_idx][i] = temp[i];
					buf_hist_cmd_size[write_idx] = temp_size;
				}
				else {
					/* handling history of commands */
					for (i = 0; i < key_idx; ++i)
						buf_hist[write_idx][i] = key_buf[i];
					buf_hist_cmd_size[write_idx] = key_idx;
				}
				/* change write idx and buf_size */
				if (buf_size < MAX_COMMANDS) buf_size++;
				write_idx = (write_idx + 1) % MAX_COMMANDS;
				/* also clear the size in buf_hist_cmd_size */
				buf_hist_cmd_size[write_idx] = 0;
				/* set up_down index to current index value */
				updown_idx = write_idx;
				temp_size = 0;
			}

			/* handling one command */
			enter_flag = 1;
			key_buf[key_idx++] = NEW_LINE;
			key_idx = 0;    // because its a new line
			leftright_idx = 0; // because its a new line 
			putc(key[scancode]); // put newline character
		}
		else if (scancode == LEFT_CTRL)
		{
			ctrl_flag = 1; // control pressed; set flag to one
		}
		else if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT)
		{
			shift_flag = 1; // shift used; set flag to one
		}
		else if (scancode == CAPS_LOCK)
		{
			capslock_flag = !capslock_flag; // change the caps flag
		}
		else
		{
			if (ctrl_flag == 0 && key_idx < KEY_BUF_SIZE_ACTUAL &&
				((scancode >= 0x01 && scancode <= 0x37) || scancode == 0x4A || scancode == 0x4E || scancode == 0x39)) // if control released and buff size is less than 128 in index
			{
				if (shift_flag == 1 && capslock_flag == 1) // if both caps lock and shift are pressed
				{
					if (shift_key[scancode] >= 65 && shift_key[scancode] <= 90) // capital letters
						temp_char = key[scancode];
					else // else case for small letters
						temp_char = shift_key[scancode];
				}
				else if (shift_flag == 1) // only shift pressed
					temp_char = shift_key[scancode];
				else if (capslock_flag == 1) // only capslock pressed
				{
					if (key[scancode] >= 97 && key[scancode] <= 122) // for small ASCII chars
						temp_char = key[scancode]-32;
					else // for capital ASCII letters
						temp_char = key[scancode];
				}
				else // else case for spamming with other letters
					temp_char = key[scancode];

				/* implement working of putting characters in between  */
				putc(temp_char);
				i = key_idx;

				while(i>leftright_idx){
					key_buf[i] = key_buf[i-1];
					--i;
				}

				for(i=leftright_idx; i<key_idx; ++i)
					putc(key_buf[i+1]);

				/* move cursor back to correct place */
				for(i=0; i<key_idx-leftright_idx; ++i)
					move_cursor_left();

				key_buf[leftright_idx] = temp_char;
				++key_idx;

				if (updown_idx == write_idx){
					for(i=0; i<key_idx; ++i)
						temp[i] = key_buf[i];
					++temp_size;
				}
				++leftright_idx;
			}
			else if (ctrl_flag == 1 && (key[scancode] == 'l' || shift_key[scancode] == 'L')) // clearing the screen
			{
				clear();  // clear the screen
				key_idx = 0; // reset buffer as everything on screen was cleared
			}
			else if (ctrl_flag == 1 && (key[scancode] == 'c' || shift_key[scancode] == 'C'))	//halt current program
				halt(0);
		}
	}
}


int32_t terminal_open(const uint8_t* filename)
{
	return 0; // just returning zero for opening term
}

int32_t terminal_read(int32_t fd, void * buf, int32_t nbytes)
{
	sti();

	enter_flag = 0;
	while (enter_flag == 0)
	{
		// nothing, keeping looping till enter is pressed
	}
	cli();
	int32_t i, j;
	for (i = 0; i < nbytes; i++) // copy over all relevant info from key_buf to buf passed in
	{
		((int8_t *)buf)[i] = key_buf[i];

		if (key_buf[i] == NEW_LINE)
			break;
	}

	for (j = i + 1; (j < nbytes && j < KEY_BUF_SIZE); j++)
		((int8_t *)buf)[j] = NULL_KEY;

	/* clear the key buffer */
	for (j = 0; j < KEY_BUF_SIZE; j++)
	{
		key_buf[j] = NULL_KEY;
	}

	// return number of bytes taken from keyboard buff
	return i + 1;
}

int32_t terminal_write(int32_t fd, const void * buf, int32_t nbytes)
{
	int32_t i;
	for (i = 0; i < nbytes; i++)
		putc(((int8_t *)buf)[i]);

	// return number of bytes written
	return i;
}

int32_t terminal_close(int32_t fd)
{
	return 0; // return zero when closing term
}
