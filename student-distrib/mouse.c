
#include "mouse.h"

/* Global variables to keep track of mouse state */
uint8_t cycle = 0;
int32_t packet_size = 3;
uint8_t packet_byte[4];
int32_t x_pixel = 0;
int32_t y_pixel = 0;
int32_t x_text = -1;
int32_t y_text = -1;

uint8_t z = 'N';


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
	outb(ENALBE_AUX, MOUSE_PORT_INFO);

	//mouse_wait(1);
	outb(GET_SB, MOUSE_PORT_INFO);

	// get status byte
	//mouse_wait(0);
	status = inb(MOUSE_PORT);
	status |= 2;
	// and resend it to mouse after modifying it
	//mouse_wait(1);
	outb(MOUSE_PORT, MOUSE_PORT_INFO);
	//mouse_wait(1);
	outb(status, MOUSE_PORT);

	 //Tell the mouse to use default settings
	mouse_wr(SET_DEFAUTLS);
  	mouse_read();  //Acknowledge

  	//Enable the mouse
	mouse_wr(ENABLE_DR);
  	mouse_read();  //Acknowledge;

	// mouse_wr(SET_SR);
	// mouse_wr(200);
	// mouse_wr(SET_SR);
	// mouse_wr(100);
	// mouse_wr(SET_SR);
	// mouse_wr(80);
	// mouse_wr(GET_MOUSE_ID);
	//
	//
 //  	//if(mouse_read() == MOUSE_ACK)
	// //{
	// 	mouse_read();
	// 	z = mouse_read();
	//   	if(z == 0x00)
	//   		packet_size = 3;
	//   	else if(z == 0x03)
	//   		packet_size = 4;
	// //}


  	enable_irq(MOUSE_IRQ);
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
	//printf("%x\n", z);

	placecolor(x_text, y_text, color);
	send_eoi(MOUSE_IRQ);

	packet_byte[cycle] = inb(MOUSE_PORT);
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

		// if(packet_size == 4)
		// 	cycle++;
		// else if(packet_size == 3)
			cycle = 0;
	}
	// else if(cycle == 3)
	// {
	// 	if(packet_size == 4)
	// 		cycle = 0;
	// 	putc('x');
	// }

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
mouse_open(const uint8_t* filename)
{
	return -1;
}

/*
uint8_t mouse_read()
input: none
output: uint8_t = one packet from mouse
*/
int32_t
mouse_read(int32_t fd, void* buf, int32_t nbytes)
{
	return inb(MOUSE_PORT);
}

/*
void mouse_write(uint8_t a_write)
input: uint8_t a_write = byte to write to mouse
output: none
purpose: write to the mouse
*/
int32_t
mouse_write(int32_t fd, const void* buf, int32_t nbytes)
{
	if (buf == NULL || nbytes != 1)
		return -1;
  	// Tell the mouse we are sending a command
  	//mouse_wait(1);
  	outb(XD4_BYTE, MOUSE_PORT_INFO);

  	// Write to the mouse
  	//mouse_wait(1);
  	outb((uint8_t)*buf, MOUSE_PORT);
}

/*
int32_t mouse_close()
input: none
output: int32_t = success or failure
purpose: to close the mouse
*/
int32_t
mouse_close(int32_t fd)
{
	return 0;
}


void
mouse_wait(uint8_t a_type) //unsigned char
{
  uint32_t _time_out=100000; //unsigned int
  if(a_type==0)
  {
    while(_time_out--) //Data
    {
      if((inb(MOUSE_PORT_INFO) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((inb(MOUSE_PORT_INFO) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

void
mouse_wr(uint8_t a_write)
{
	outb(XD4_BYTE, MOUSE_PORT_INFO);
  	outb(a_write, MOUSE_PORT);
}
