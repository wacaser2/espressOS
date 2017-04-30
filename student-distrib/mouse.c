
#include "mouse.h"

uint8_t cycle = 0;
uint8_t packet_byte[3];
int32_t x_pixel = 0;
int32_t y_pixel = 0;
int32_t x_text = -1;
int32_t y_text = -1;
int32_t x_temp = 0;
int32_t y_temp = 0;
uint8_t prev_y = 0;

volatile int left_flag = 0;
volatile int right_flag = 0;
volatile int middle_flag = 0;


void
mouse_init(void)
{
	unsigned char status;

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


void
mouse_handler(void)
{
	//clear();
	send_eoi(12);

	//printf("%d", cycle);
	/*if(cycle == 0)
	{
		packet_byte[0] = inb(0x60);		// first IRQ
		cycle++;
		puts("cycle3\n");
	}
	else if(cycle == 1)
	{
		packet_byte[1] = inb(0x60);		// second IRQ
		cycle++;
		puts("cycle1");
	}
	else if(cycle == 2)
	{
		packet_byte[2] = inb(0x60);		// third IRQ
		x_move = packet_byte[1];
		y_move = packet_byte[2];
		puts("cycle2");
		//puts("x_move");
		cycle = 0;
	}*/

	placecolor(x_text, y_text, ATTRIB);
	//placec(x_text, y_text, 0x70, 'M');

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
		  	
		if (prev_y & 0x20)
		{
			y_temp |= (-256); //delta-y is a negative value
		}
		  	
		if (packet_byte[1] & 0x10)
		{
			x_temp |= (-256); //delta-x is a negative value
		}
		  	
		if (packet_byte[1] & 0x4){
			//puts("Middle button is pressed!n");
			middle_flag = 1;
		}
		else if (packet_byte[1] & 0x2)
		{
			//puts("Right button is pressed!n");
			right_flag = 1;
		}
		else if (packet_byte[1] & 0x1)
		{   	
		  	//puts("Left button is pressed!n");
		  	left_flag = 1;
		}
		else
		{
			//clear();
			//puts("ha");
			x_pixel += x_temp;
			y_pixel -= y_temp;
			x_text = (x_pixel >> 2);
			y_text = (y_pixel >> 3);
			if(x_text < 0)
			{
				//printf("  %d  %d     \n", x_text, y_text);
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
			//printf("  %d  %d    \n", x_text, y_text);
			//printf(" %d     %d    %x  \n", x_temp, y_temp, packet_byte[1]);
		}

		//y_text = prev_y;
		prev_y = packet_byte[1];
	}
 	//printf(" %d     %d    %x  \n", x_temp, y_temp, packet_byte[1]);    

    //placec(x_text, y_text, 0x70, 'M');
    placecolor(x_text, y_text, 0x70);
    //printf("  %d  %d     ", x_text, y_text);
    	
    // do what you want here, just replace the puts's to execute an action for each button
    // to use the coordinate data, use mouse_bytes[1] for delta-x, and mouse_bytes[2] for delta-y

    //printf("%d  %d     ", x_move, y_move);
}


int32_t
mouse_open(void)
{
	return 0;
}

unsigned char
mouse_read(void)
{
	//mouse_wait(0);

	return inb(0x60);
}

void mouse_write(unsigned char a_write)
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

int32_t
mouse_close(void)
{
	return 0;
}


/*void 
mouse_wait(unsigned char type)
{
  	unsigned int time_out = 100000; //unsigned int

  	if(type == 0)
  	{
    	while(time_out--) //Data
    	{
    		if((inb(0x64) & 1) == 1)	// make sure that bit 0 is set
    		{
        		return;
      		}
    	}
  	}
  	else if(type == 1)
  	{	
    	while(time_out--) //Signal
    	{	
      		if((inb(0x64) & 2) == 0)	// make sure that bit 1 is cleared
      		{
        		return;
      		}
    	}
  	}
}*/
