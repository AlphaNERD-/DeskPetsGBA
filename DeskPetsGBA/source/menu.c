#include <gba.h>
#include <maxmod.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "soundbank.h"
#include "soundbank_bin.h"

#include "dpVariants.h"

char *getDeviceTypeText(int dpModel);
void buildMenu();
void makeMenuEntry(int row, char* caption, char* content, bool enabled);
bool isColorAvailable(int dpModel);
void getColorsForModel(int dpModel);
char *getColorText(int dpModel);
bool isChannelAvailable(int dpModel);

int deskpetModel = 0;
int deskpetColor = 0;
int deskpetChannel = 0;
int deskpetTeamColor = 0;
int controlVariant = 0;
int flipSignals = 0;

const int consoleLength = 30;

int availableMenuEntries[6];
int currentMenuEntry;

enum MENU_ENTRIES
{
	ENTRY_WRAP=-1,
	ENTRY_MODEL,
	ENTRY_COLOR,
	ENTRY_CHANNEL,
	ENTRY_TEAMCOLOR,
	ENTRY_CONTROLVARIANT,
	ENTRY_FLIPSIGNAL
};

int availableColors[7];

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
	
	//iprintf("\x1b[3;0HUwU *nuzzles your entry point*");
	
	int keys_pressed, keys_released;
	bool updateMenu = false;
	
	buildMenu();
	
	do {
		scanKeys();
		
		keys_pressed = keysDown();
		keys_released = keysUp();
		
		if (keys_pressed & KEY_LEFT)
		{
			deskpetModel = deskpetModel - 1;
			
			updateMenu = true;
		}
		
		if (keys_pressed & KEY_RIGHT)
		{
			deskpetModel = deskpetModel + 1;
			
			updateMenu = true;
		}
		
		if (updateMenu)
		{
			buildMenu();
			
			updateMenu = false;
		}
	} while( 1 );
}

void buildMenu()
{
	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	iprintf("\x1b[0;5HDeskPets Controller");
	
	int currentRow = 3;
	
	makeMenuEntry(currentRow, "Model:", getDeviceTypeText(deskpetModel), true);
	
	if (isColorAvailable(deskpetModel))
	{
		currentRow = currentRow + 1;
		
		getColorsForModel(deskpetModel);
		
		makeMenuEntry(currentRow, "Color", getColorText(availableColors[deskpetColor]), false);
	}
	else
	{
		availableColors[0] = 0;
		availableColors[1] = 0;
		availableColors[2] = 0;
		availableColors[3] = 0;
		availableColors[4] = 0;
		availableColors[5] = 0;
		availableColors[6] = 0;
		deskpetColor = 0;
	}
	
	if (isChannelAvailable(deskpetModel))
	{
		currentRow = currentRow + 1;
		
		deskpetChannel = 1;
		
		switch(deskpetChannel)
		{
			case 1:
				makeMenuEntry(currentRow, "Channel:", "A", false);
				break;
			case 2:
				makeMenuEntry(currentRow, "Channel:", "B", false);
				break;
			case 3:
				makeMenuEntry(currentRow, "Channel:", "C", false);
				break;
		}
	}
	else
	{
		deskpetChannel = 0;
	}
}

void makeMenuEntry(int row, char* caption, char* content, bool active)
{
	char str[consoleLength];
	char offsetCol[3];
	char offsetRow[3];

	strcpy(str, "\x1b[");
	sprintf(offsetRow, "%d", row);
	strcat(str, offsetRow);
	strcat(str, ";0H");
	
	if (active)
		strcat(str, "> ");
	
	strcat(str, caption);
	iprintf(str);
	
	strcpy(str, "\x1b[");
	strcat(str, offsetRow);
	strcat(str, ";");
	sprintf(offsetCol, "%d", consoleLength - strlen(content));
	strcat(str, offsetCol);
	strcat(str, "H");
	strcat(str, content);
	iprintf(str);
}

bool isChannelAvailable(int dpModel)
{
	if (dpModel == MODEL_BATTLETANKV2 |
		dpModel == MODEL_TANKBOTV2 |
		dpModel == MODEL_BATTLETANKCHANNEL |
		dpModel == MODEL_CARBOT |
		dpModel == MODEL_DRIFTBOT)
		{
			return true;
		}
		
	return false;
}

bool isColorAvailable(int dpModel)
{
	if (dpModel == MODEL_BATTLETANK |
		dpModel == MODEL_SKITTERBOT |
		dpModel == MODEL_TANKBOT |
		dpModel == MODEL_TANKBOTFIRE |
		dpModel == MODEL_TREKBOT)
		{
			return true;
		}
		
	return false;
}

void getColorsForModel(int dpModel)
{
	switch (dpModel)
	{
		case MODEL_BATTLETANK:
			availableColors[0] = COLOR_OLIVE;
			availableColors[1] = COLOR_GREEN;
			availableColors[2] = ENTRY_WRAP;
			availableColors[3] = ENTRY_WRAP;
			availableColors[4] = ENTRY_WRAP;
			availableColors[5] = ENTRY_WRAP;
			availableColors[6] = ENTRY_WRAP;
			break;
		case MODEL_TANKBOT:
			availableColors[0] = COLOR_BLUE;
			availableColors[1] = COLOR_GREEN;
			availableColors[2] = COLOR_GOLD;
			availableColors[3] = COLOR_ORANGE;
			availableColors[4] = COLOR_GREY;
			availableColors[5] = COLOR_WHITE;
			availableColors[6] = ENTRY_WRAP;
			break;
		case MODEL_TANKBOTFIRE:
			availableColors[0] = COLOR_BLUE;
			availableColors[1] = COLOR_GREEN;
			availableColors[2] = COLOR_GREY;
			availableColors[3] = COLOR_RED;
			availableColors[4] = COLOR_WHITE;
			availableColors[5] = ENTRY_WRAP;
			availableColors[6] = ENTRY_WRAP;
			break;
		case MODEL_TREKBOT:
			availableColors[0] = COLOR_YELLOW;
			availableColors[1] = COLOR_BLUE;
			availableColors[2] = COLOR_BLACK;
			availableColors[3] = COLOR_RED;
			availableColors[4] = COLOR_WHITE;
			availableColors[5] = COLOR_GOLD;
			availableColors[6] = COLOR_CLEAR;
			break;
		case MODEL_SKITTERBOT:
			availableColors[0] = COLOR_YELLOW;
			availableColors[1] = COLOR_GREEN;
			availableColors[2] = COLOR_BLUE;
			availableColors[3] = COLOR_RED;
			availableColors[4] = COLOR_CLEAR;
			availableColors[5] = ENTRY_WRAP;
			availableColors[6] = ENTRY_WRAP;
			break;
			
	}
	
}

char *getDeviceTypeText(int dpModel)
{
	switch (dpModel)
	{
		case MODEL_DRIFTBOT:
			return "Driftbot";
		case MODEL_BATTLETANK:
			return "Battletank";
		case MODEL_BATTLETANKCHANNEL:
			return "Battletank (ABC Switch)";
		case MODEL_BATTLETANKV2:
			return "Battletank (Team Color)";
		case MODEL_CARBOT:
			return "Carbot";
		case MODEL_TANKBOT:
			return "Tankbot";
		case MODEL_TANKBOTFIRE:
			return "Tankbot (Fire Support)";
		case MODEL_TANKBOTV2:
			return "Tankbot V2";
		case MODEL_TREKBOT:
			return "Trekbot";
		case MODEL_SKITTERBOT:
			return "Skitterbot";
	}
	
	return "";
};

char *getColorText(int dpColor)
{
	switch (dpColor)
	{
		case COLOR_OLIVE:
			return "Olive";
		case COLOR_GREEN:
			return "Green";
		case COLOR_BLUE:
			return "Blue";
		case COLOR_GOLD:
			return "Gold";
		case COLOR_ORANGE:
			return "Orange";
		case COLOR_GREY:
			return "Grey";
		case COLOR_WHITE:
			return "White";
		case COLOR_YELLOW:
			return "Yellow";
		case COLOR_BLACK:
			return "Black";
		case COLOR_RED:
			return "Red";
		case COLOR_CLEAR:
			return "Clear";
	}
	
	return "";
};