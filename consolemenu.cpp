/*
    Copyright 2008 David House

    This file is part of Microsight.

    Microsight is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Microsight is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Microsight.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <conio.h>
#include <windows.h>
#include <iostream>


void drawMainMenu(int pos);

void gotoxy(int x, int y)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void main()
{
	system("cls");

	int keyboard_read=0; // integer to store input from the keyboard
	int PASS=1;          // integer used to signal the application to close
	int currpos=1;       // integer to store the selection from the menu

	drawMainMenu(1); // draw the main menu with selection at option 1

	while (PASS)
	{
		keyboard_read=getch();  // get input from keyboard
		if (keyboard_read == 0) // wait for the input to be given
		{
			keyboard_read = getch();
		}

		switch(keyboard_read) // check the input against these values
		{
			case 72: // if the Up cursor key is pressed
			       if(currpos > 1)
			       {
				  currpos=currpos-1; // move the selection 1 step up
			       }
			       drawMainMenu(currpos); // draw menu with the new selection value
			break;

			case 80: // if the Down cursor key is pressed
				if(currpos >= 1 && currpos < 5)
				{
				  currpos=currpos+1; // move the selection one step down
				}
				drawMainMenu(currpos); // draw menu with new selection value
			break;

			case 13: // if the Return(Enter) key is pressed
				if(currpos==5)  // is the exit command highlighted?
				{
				   PASS=0;      // end the input sequence if yes
				}
			break;

		}
	}
}


void drawMainMenu(int pos)
{

	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	cursor.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
   int current=pos; // store the value of the currently highlighted selection

   gotoxy(25,4); // position on screen
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
   cprintf("        BU MicroSight        "); // name of menu item
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

   if (current == 1)
   {

	 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
    // turn text color to black
      // turn the text background color grey

     gotoxy(25,7); // position on screen
     cprintf("-> Alter System Variables    "); // name of menu item

	 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    // textcolor(7);  // turn the text colour back to normal
    // textbackground(0);  // turn the text BG to black
   }
   else
   {
     gotoxy(25, 7);
     cprintf("   Alter System Variables    ");
   }

   if(current == 2)
   {
     //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
    // textbackground(7);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

     gotoxy(25, 9);
     cprintf("-> Edit Trial Details        ");

	 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
     //textcolor(7);
     //textbackground(0);

   }
   else
   {
     gotoxy(25, 9);
     cprintf("   Edit Trial Details        ");
   }

   if(current == 3)
   {

	 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
  
     gotoxy(25, 11);
     cprintf("   Set Segmentation Method   ");

	 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		
   }
   else
   {

     gotoxy(25, 11);
     cprintf("   Set Segmentation Method   ");

   }

   if (current == 4)
   {
      
	 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

     gotoxy(25, 13);
     cprintf("   Begin Trial Now           ");

	 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

   }
   else
   {

     gotoxy(25, 13);
     cprintf("   Begin Trial Now           ");

   }


   if(current == 5)
   {
     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

     gotoxy(25, 19);
     cprintf("   Exit                      ");

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

   }
   else
   {

     gotoxy(25, 19);
     cprintf("   Exit                      ");

   }

}
