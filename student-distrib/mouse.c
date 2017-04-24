
#include "mouse.h"

unsigned char cycle = 0;
char packet_byte[3];
char x_move = 0;
char y_move = 0;


void
mouse_init(void)
{
	unsigned char status;
	//mouse_wait(1);
	outb(0x20, 0x64);
	//mouse_wait(0);
	status = inb(0x60);		// get status byte
	status |= 2;
	status &= 223; 
	//mouse_wait(1);
	outb(0x60, 0x64);
	outb(status, 0x60);

	// Enable the auxiliary mouse device
	//mouse_wait(1);
	outb(0xA8, 0x64);

	 //Tell the mouse to use default settings
  	mouse_write(0xF6);
  	mouse_read();  //Acknowledge
  
  	//Enable the mouse
  	mouse_write(0xF4);
  	mouse_read();  //Acknowledge

  	enable_irq(12);
}


void
mouse_handler(void)
{
	if(cycle == 0)
	{
		packet_byte[1] = inb(0x60);		// first IRQ
		cycle++;
	}
	else if(cycle == 1)
	{
		packet_byte[2] = inb(0x60);		// second IRQ
		cycle++;
	}
	else if(cycle == 2)
	{
		packet_byte[0] = inb(0x60);		// third IRQ
		x_move = packet_byte[2];
		y_move = packet_byte[0];
		cycle = 0;
	}
}


int32_t
mouse_open(void)
{
	return 0;
}

char
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
