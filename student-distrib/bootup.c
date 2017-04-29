#include "bootup.h"



void bootup_sequence_1()
{
	clear();

	setcolor(BACKGROUND);
	char message[] = "\n                                                   ____   _____\n                                                  / __ \\ / ____|\n                 ___  ___ _ __  _ __ ___  ___ ___| |  | | (___  \n                / _ \\/ __| '_ \\| '__/ _ \\/ __/ __| |  | |\\___ \\ \n               |  __/\\__ \\ |_) | | |  __/\\__ \\__ \\ |__| |____) |\n                \\___||___/ .__/|_|  \\___||___/___/\\____/|_____/ \n                         | |                                    \n                         |_|                                    \n\0";
	char message2[] = "\n\n\n\n                                _____________\n                               <_____________> ___\n                               |             |/ _ \\\n                               |               | | |\n                               |               |_| |\n                            ___|             |\\___/\n                           /    \\___________/    \\\n                           \\_____________________/\n\0";	
	
	int ctr;
	for(ctr=0; message[ctr]!='\0'; ctr++)
		putspecial(message[ctr]);
	for(ctr=0; message2[ctr]!='\0'; ctr++)
	 	putspecial(message2[ctr]);


	int view_flag = 0;
	char* video_mem = (char *)VIDEO;

	int py;
	int xc[] = {10, 11, 12};
	int yc[] = {31, 35, 39, 43, 46};

	cli();
 	int x,x1,x2;
	for(x2=0; x2<1000; x2++)
	{	
		for(x1=0; x1<10000; x1++)
		{
			for(x=0; x<30; x++)
			{
				if(view_flag == 0 && x1 % 20 ==0)
				{
					view_flag = !view_flag;
					*(uint8_t *)(video_mem + ((NUM_COLS*(xc[x/10]) + yc[py%5]) << 1)) = '(';
					*(uint8_t *)(video_mem + ((NUM_COLS*(xc[x/10]) + yc[py%5]) << 1) + 1) = TEXT;	
					py++;			
				}
				else if(x1 % 20 ==0)
				{
					view_flag = !view_flag;
					*(uint8_t *)(video_mem + ((NUM_COLS*(xc[x/10]) + yc[py%5]) << 1)) = ')';
					*(uint8_t *)(video_mem + ((NUM_COLS*(xc[x/10]) + yc[py%5]) << 1) + 1) = TEXT;	
					py++;
				}
			}	
			py = 0;
		}
	}

	sti();		


	clear(); 
}
