#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "keyboard.h"

volatile int enter_flag = 0;
volatile int ctrl_flag = 0;
volatile int shift_flag = 0;
volatile int capslock_flag = 0;
volatile int8_t key_buf[KEY_BUF_SIZE];
volatile int key_idx = 0;

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

	/* Read from the keyboard's data buffer */
	scancode = inb((int)KEYBOARD_PORT);

	send_eoi(KEYBOARD_IRQ);	//allow more interrupts to queue for the keyboard
	/* If the top bit of the byte we read from the keyboard is
	*  set, that means that a key has just been released */
	if (scancode & 0x80)
	{
		/* You can use this one to see if the user released the
		  *  shift, alt, or control keys... */
		  //return; // for now
		if (scancode == LEFT_CTRL_REL)
		{
			/* set control flag to zero when key is released */
			ctrl_flag = 0;
		}
		if (scancode == LEFT_SHIFT_REL || scancode == RIGHT_SHIFT_REL)
		{
			/* set shift flag to zero when key is released */
			shift_flag = 0;
		}
	}
	else
	{
		if (scancode == BACKSPACE) // case for backspace
		{
			if (key_idx > 0)
			{
				key_buf[--key_idx] = NULL_KEY;
				backspace_put(key_idx); // call backspace_put func
			}
		}
		else if (scancode == ENTER || key[scancode] == CARRIAGE_RETURN)
		{
			enter_flag = 1;
			key_buf[key_idx++] = NEW_LINE;
			key_idx = 0;    // because its a new line
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
			if (ctrl_flag == 0 && key_idx < KEY_BUF_SIZE_ACTUAL) // if control released and buff size is less than 128
			{
				if ((scancode >= 0x01 && scancode <= 0x37) || scancode == 0x4A || scancode == 0x4E || scancode == 0x39)
				{
					if (shift_flag == 1 && capslock_flag == 1) // if both caps lock and shift are pressed
					{
						if (shift_key[scancode] >= 65 && shift_key[scancode] <= 90) // capital letters
						{
							key_buf[key_idx++] = key[scancode];
							putc(key[scancode]);
						}
						else // else case for small letters
						{
							key_buf[key_idx++] = shift_key[scancode];
							putc(shift_key[scancode]);
						}
					}
					else if (shift_flag == 1) // only shift pressed
					{
						key_buf[key_idx++] = shift_key[scancode];
						putc(shift_key[scancode]);
					}
					else if (capslock_flag == 1) // only capslock pressed
					{
						if (key[scancode] >= 97 && key[scancode] <= 122) // for small ASCII chars
						{
							key_buf[key_idx++] = key[scancode] - 32;
							putc(key[scancode] - 32);
						}
						else // for capital ASCII letters
						{
							key_buf[key_idx++] = key[scancode];
							putc(key[scancode]);
						}
					}
					else // else case for spamming with other letters
					{
						key_buf[key_idx++] = key[scancode];
						putc(key[scancode]);  // put the char on the screen
					}
				}
			}
			else if (ctrl_flag == 1 && (key[scancode] == 'l' || shift_key[scancode] == 'L')) // clearing the screen
			{
				clear();  // clear the screen
				key_idx = 0; // reset buffer as everything on screen was cleared
				//puts("[user@espressOS:/ ]$ ");
			}
		}
	}
}


int32_t terminal_open(const uint8_t* filename)
{
	return 0; // just returning zero for opening term
}

int32_t terminal_read(int32_t fd, void * buf, int32_t nbytes)
{
	//puts("[user@espressOS:/ ]$ ");
	sti();

	enter_flag = 0;
	while (enter_flag == 0)
	{
		// nothing, keeping looping till enter is pressed
	}
	cli();
	int i, j;

	for (i = 0; i < nbytes; i++) // copy over all relevant info from key_buf to buf passed in
	{
		((int8_t *)buf)[i] = key_buf[i];

		if (key_buf[i] == NEW_LINE)
			break;
	}

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
	//puts("[user@espressOS:/ ]$ ");

	int i;
	for (i = 0; i < nbytes; i++)
		putc(((int8_t *)buf)[i]);

	// return number of bytes written
	return i;
}

int32_t terminal_close(int32_t fd)
{
	return 0; // return zero when closing term
}
