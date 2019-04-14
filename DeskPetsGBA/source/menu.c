#include <gba.h>
#include <maxmod.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "soundbank.h"
#include "soundbank_bin.h"

#include "dpVariants.h"

char *getDeviceTypeText(int dpModel);

static int deskpetModel = 0;
static int deskpetColor = 0;
static int deskpetChannel = 0;
static int deskpetTeamColor = 0;
static int controlVariant = 0;
static int flipSignals = 0;

const int consoleLength = 30;

int main() 
{
	consoleInit(	
		0,		// charbase
		4,		// mapbase
		0,		// background number
		NULL,	// font
		0, 		// font size
		15		// 16 color palette
	);
	
	BG_COLORS[0]=RGB8(10,10,10);
	BG_COLORS[241]=RGB5(31,31,31);

	SetMode(MODE_0 | BG0_ON);

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	iprintf("\x1b[0;5HDeskPets Controller");
	//iprintf("\x1b[3;0HUwU *nuzzles your entry point*");
	
	char str[consoleLength];
	strcpy(str, "\x1b[3;0H");
	strcat(str, getDeviceTypeText(0));
	iprintf(str);

	do {

		int keys_pressed, keys_released;
		
		VBlankIntrWait();
		mmFrame();
	 
		scanKeys();

		keys_pressed = keysDown();
		keys_released = keysUp();
	} while( 1 );
}

void buildMenu
{
	char str[consoleLength];
	
	strcpy(str, "\x1b[3;0H");
	strcat(str, getDeviceTypeText(deskpetModel));
	iprintf(str);
}


char *getDeviceTypeText(int dpModel)
{
	switch (dpModel)
	{
		case MODEL_DRIFTBOT:
		return "Driftbot";
	}
	
	return "";
};