#include "bootup.h"



void bootup_sequence_1()
{
	clear();

	char message[] = "\n                                                   ____   _____\n                                                  / __ \\ / ____|\n                 ___  ___ _ __  _ __ ___  ___ ___| |  | | (___  \n                / _ \\/ __| '_ \\| '__/ _ \\/ __/ __| |  | |\\___ \\ \n               |  __/\\__ \\ |_) | | |  __/\\__ \\__ \\ |__| |____) |\n                \\___||___/ .__/|_|  \\___||___/___/\\____/|_____/ \n                         | |                                    \n                         |_|                                    \n\0";
	char message2[] = "\n\n\n\n                                _____________\n                               <_____________> ___\n                               |             |/ _ \\\n                               |               | | |\n                               |               |_| |\n                            ___|             |\\___/\n                           /    \\___________/    \\\n                           \\_____________________/\n\0";	
	
	int ctr;
	for(ctr=0; message[ctr]!='\0'; ctr++)
		fputc(message[ctr]);
	for(ctr=0; message2[ctr]!='\0'; ctr++)
	 	fputc(message2[ctr]);
	setcolor(TEXT);


	int view_flag = 0;

	int py;
	int xc[] = {10, 11, 12};
	int yc[] = {31, 35, 39, 43, 46};

	cli();
 	int x,x1,x2;
	for(x2=0; x2<100; x2++)
	{	
		for(x1=0; x1<10000; x1++)
		{
			for(x=0; x<30; x++)
			{
				if(view_flag == 0 && x1 % 20 ==0)
				{
					view_flag = !view_flag;
					fplacec(yc[py % 5], xc[x / 10], TEXT, '(');
					py++;			
				}
				else if(x1 % 20 ==0)
				{
					view_flag = !view_flag;
					fplacec(yc[py % 5], xc[x / 10], TEXT, '}');
					py++;
				}
			}	
			py = 0;
		}
	}

	sti();		


	clear(); 
}
