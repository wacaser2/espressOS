
#include "mouse.h"

/* Global variables to keep track of mouse state */
uint8_t cycle = 0;
uint8_t packet_byte[3];
int32_t x_pixel = 0;
int32_t y_pixel = 0;
int32_t x_text = -1;
int32_t y_text = -1;

/* Temporary variables */
int32_t x_temp = 0;
int32_t y_temp = 0;
uint8_t prev_y = 0;
int8_t color = 0;

/* Flags for mouse status */
int32_t left_flag = 0;
int32_t right_flag = 0;
int32_t middle_flag = 0;

/*
void mouse_init()
input: none
output: none
purpose: initializes the mouse
*/
void
mouse_init(void)
{
	uint8_t status;

	// Enable the auxiliary mouse device
	//mouse_wait(1);
	outb(0xA8, 0x64);

	//mouse_wait(1);
	outb(0x20, 0x64);
	//mouse_wait(0);
	status = inb(0x60);		// get status byte
	status |= 2;
	//printf("%d\n", status);
	//status &= 223; 
	//mouse_wait(1);
	outb(0x60, 0x64);
	outb(status, 0x60);

	 //Tell the mouse to use default settings
  	mouse_write(0xF6);
  	mouse_read();  //Acknowledge
  	//printf("Ack1");
  
  	//Enable the mouse
  	mouse_write(0xF4);
  	mouse_read();  //Acknowledge
  	//printf("Ack2");

  	enable_irq(12);
}

/*
void mouse_handler()
input: none
output: none
purpose: to handle mouse interrupts
*/
void
mouse_handler(void)
{
	send_eoi(12);

	placecolor(x_text, y_text, color);

	packet_byte[cycle] = inb(0x60);
	if(cycle == 0)
	{
		cycle++;
	}
	else if(cycle == 1)
	{
		cycle++;
	}
	else if(cycle == 2)
	{
		x_temp = packet_byte[2];
		y_temp = packet_byte[0];
		cycle = 0;
		if ((packet_byte[1] & 0x80) || (packet_byte[1] & 0x40))
		{
			return; // the mouse only sends information about overflowing, do not care about it and return
		}
		  	
		if (prev_y & 0x20)	//sign extend
		{
			y_temp |= (-256); //delta-y is a negative value
		}
		  	
		if (packet_byte[1] & 0x10)	//sign extend
		{
			x_temp |= (-256); //delta-x is a negative value
		}
		  	
		if (packet_byte[1] & 0x4){
			//puts("Middle button is pressed!n");
			middle_flag = 1;
		}
		if (packet_byte[1] & 0x2)
		{
			//puts("Right button is pressed!n");
			right_flag = 1;
		}
		if (packet_byte[1] & 0x1)
		{   	
		  	//puts("Left button is pressed!n");
		  	left_flag = 1;
		}
		x_pixel += x_temp;
		y_pixel -= y_temp;
		x_text = (x_pixel >> 2);
		y_text = (y_pixel >> 3);
		if(x_text < 0)
		{
			x_text = 0;
			x_pixel = 0;
		}
		else if(x_text > 79)
		{
			x_text = 79;
			x_pixel = (((x_text + 1) << 2) - 1);
		}

		if(y_text < 0)
		{
			y_text = 0;
			y_pixel = 0;
		}
		else if(y_text > 24)
		{
			y_text = 24;
			y_pixel = (((y_text + 1) << 3) - 1);
		}
		prev_y = packet_byte[1];
	}

    color = getcolor(x_text, y_text);
    placecolor(x_text, y_text, (color ^ 0x88));
}

/*
int32_t mouse_open()
input: none
output: success or failure
purpose to open the mouse to use
*/
int32_t
mouse_open(void)
{
	return -1;
}

/*
uint8_t mouse_read()
input: none
output: uint8_t = one packet from mouse
*/
uint8_t
mouse_read(void)
{
	//mouse_wait(0);

	return inb(0x60);
}

/*
void mouse_write(uint8_t a_write)
input: uint8_t a_write = byte to write to mouse
output: none
purpose: write to the mouse
*/
void mouse_write(uint8_t a_write)
{
  	//Wait to be able to send a command
  	//mouse_wait(1);
  	//Tell the mouse we are sending a command
  	outb(0xD4, 0x64);
  	//Wait for the final part
  	//mouse_wait(1);
  	//Finally write
  	//printf("%d\n", a_write);
  	outb(a_write, 0x60);
}

/*
int32_t mouse_close()
input: none
output: int32_t = success or failure
purpose: to close the mouse
*/
int32_t
mouse_close(void)
{
	return 0;
}
