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

static unsigned char key[128] =
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


static unsigned char shift_key[128] =
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
	//unsigned char scancode;

    /* Read from the keyboard's data buffer */
    //scancode = inb((int)KEYBOARD_PORT);

    char scancode = 0;
    do
    {
      if(inb((int)KEYBOARD_PORT) != scancode)
      {
        scancode = inb((int)KEYBOARD_PORT);
        if(scancode >= 0x01)
        {
          break;
        }
      }
    }while(1);
    
    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if(scancode & 0x80)
    {
      /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
      //return; // for now
      if(scancode == LEFT_CTRL_REL)
      {
        ctrl_flag = 0;
      }
      if(scancode == LEFT_SHIFT_REL || scancode == RIGHT_SHIFT_REL)
      {
        shift_flag = 0;
      }
      if(scancode == CAPS_LOCK_REL)
      {
        capslock_flag = 0;
      }
    }
    else
    {
      if(scancode == BACKSPACE)
      {
        if(key_idx > 0)
        {
          //key_idx--;
          key_buf[--key_idx] = NULL_KEY;
          backspace_put();
        }
      }
      else if(scancode == ENTER)
      {
        enter_flag = 1;
        //key_idx++;
        key_buf[key_idx++] = NEW_LINE;
        enter_put();
      }
      else if(scancode == LEFT_CTRL)
      {
        ctrl_flag = 1;
      }
      else if(scancode == LEFT_SHIFT || scancode == RIGHT_SHFT)
      {
        shift_flag = 1;
      }
      else if(scancode == CAPS_LOCK)
      {
        capslock_flag = 1;
      }
      else
      {
        if(ctrl_flag == 0 && key_idx < KEY_BUF_SIZE)
        {
          if((scancode >= 0x01 && scancode <= 0x37) || scancode == 0x4A || scancode == 0x4E)
          {
            if(shift_flag == 1 && capslock_flag == 1)
            {
              if(shift_key[scancode] >= 65 && shift_key[scancode] <= 90)
              {
                key_buf[key_idx++] = key[scancode];
                putc(key[scancode]);
              }
              else
              {
                key_buf[key_idx++] = shift_key[scancode];
                putc(shift_key[scancode]);
              }
            }
            else if(shift_flag == 1)
            {
              key_buf[key_idx++] = shift_key[scancode];
              putc(shift_key[scancode]);
            }
            else if(capslock_flag = 1)
            {
              if(key[scancode] >= 97 && shift_key[scancode] <= 122)
              {
                key_buf[key_idx++] = key[scancode];
                putc(key[scancode] - 32);
              }
              else
              {
                key_buf[key_idx++] = key[scancode];
                putc(key[scancode]);
              }
            }
            else
            {
              key_buf[key_idx++] = key[scancode];
              putc(key[scancode]);  // put the char on the screen
            }
          }
        }
        else if(ctrl_flag == 1 && scancode == 'l')
        {
          clear();  // clear the screen
        }
      }
    }
    
    send_eoi(KEYBOARD_IRQ);	//allow more interrupts to queue for the keyboard
    
    
    /*else
    {
        putc(key[scancode]);	// put the char on the screen
    }*/

}


int32_t terminal_open(void)
{
  return 0;
}

int32_t terminal_read(void * buf, int32_t nbytes)
{
  enter_flag = 0;
  while(enter_flag == 0)
  {
    // nothing, keeping looping till enter is pressed

  }

  int i;
  for(i = 0; i < nbytes; i++)
  {
      if(key_buf[i] == NULL_KEY)
      {
        break;
      }
      buf[i] = key_buf[i];
  }

  /* clear the key buffer */
  for(i = 0; i < KEY_BUF_SIZE; i++)
  {
      key_buf[i] = NULL_KEY;
  }

}

int32_t terminal_write(const void * buf, int32_t nbytes)
{
  /*int i = 0;
  int32_t byte_count = 0;
  for(i = 0; i < nbytes; i++)
  {
    putc((uint8_t *)buf[i]);
    byte_count++;
  }
   
  if(byte_count != nbytes)
  {
    return -1;
  } */
  printf("%s\n", (char *)buf);

  return nbytes;
}

int32_t terminal_close(void)
{
  return 0;
}