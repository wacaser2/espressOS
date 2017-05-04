#include "bootup.h"

/*
void bootup_sequence_1()
input: none
output: none
purpose: the lulz
*/
void bootup_sequence_1()
{
	cli();

	clear();

	char message[] = "\n                                                   ____   _____\n                                                  / __ \\ / ____|\n                 ___  ___ _ __  _ __ ___  ___ ___| |  | | (___  \n                / _ \\/ __| '_ \\| '__/ _ \\/ __/ __| |  | |\\___ \\ \n               |  __/\\__ \\ |_) | | |  __/\\__ \\__ \\ |__| |____) |\n                \\___||___/ .__/|_|  \\___||___/___/\\____/|_____/ \n                         | |                                    \n                         |_|                                    \n\0";
	char message2[] = "\n\n\n\n                                _____________\n                               <_____________> ___\n                               |             |/ _ \\\n                               |               | | |\n                               |               |_| |\n                            ___|             |\\___/\n                           /    \\___________/    \\\n                           \\_____________________/\n\0";

	int32_t ctr;
	for(ctr=0; message[ctr]!='\0'; ctr++)
		fputc(message[ctr]);
	for(ctr=0; message2[ctr]!='\0'; ctr++)
	 	fputc(message2[ctr]);
	setcolor(TEXT);


	int32_t view_flag = 0;

	int32_t py;
	int32_t xc[] = {10, 11, 12};
	int32_t yc[] = {31, 35, 39, 43, 46};

	 	int32_t x,x1,x2;
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


	clear();
}

/*
void login_screen()
input: none
output: none
purpose: to keep people who don't know the username and password off
*/
void login_screen()
{

	int32_t login = FAILURE;
	set_login_flag(2);
	set_login_mode(FAILURE);
	set_password_being_entered_mode(SUCCESS);
	char message[] = "$$$$$$$$  Enter username: ";
	char message2[] = "$$$$$$$$  Enter password: ";

	clear();
	puts("\n\n\n\n\n\n\n\n\n");
	while(login == FAILURE)
	{
		puts(message);

		// user entry
		char buf[20], buf2[20];
		terminal_read(0, buf, 20);
		setlinecolor(0xB);

		puts("\n\n");
		puts(message2);
		set_password_being_entered_mode(FAILURE);
		terminal_read(0, buf2, 20);

		if(strcmp(buf2, PASSWORD) == 0 &&
		   strcmp(buf, USER_NAME) == 0)
		{
			// successful
			cli();
			login = SUCCESS;
			set_login_flag(SUCCESS);
			set_login_mode(SUCCESS);
			clear();
			puts("\n\n\n\n\n\n\n\n\n");
		}
		else
		{
			// fail, print error message to screen
			login = FAILURE;

			clear();
			puts("\n\n\n\n\n\n");
			puts("                        WRONG PASSWORD OR USERNAME!\n");
			setlinecolor(0x4);
			puts("\n\n");
		}

		set_password_being_entered_mode(SUCCESS);

	}

	clear();
}
