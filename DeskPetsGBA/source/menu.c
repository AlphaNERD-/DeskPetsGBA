#include <gba.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "dpVariants.h"

#include "tankController.h"
;
void buildMenu();
void makeMenuEntry(int row, char* caption, char* content, bool enabled);
bool isColorAvailable();
char *getColorText();
bool isChannelAvailable();
void menuUp();
void menuDown();
void menuLeft();
void menuRight();
bool isAlternativeControlVariantAvailable();
char *getControlTypeText();
char *getModelText();

static int deskpetModel = 0;
static int deskpetColor = 0;
static int deskpetChannel = 0;
static int controlVariant = 0;
static int flipSignals = 0;

const int consoleLength = 30;
 
int currentMenuIndex = 0;
int currentMenuEntry = 0;

enum MENU_ENTRIES
{
	ENTRY_WRAP=-1,
	ENTRY_MODEL,
	ENTRY_COLOR,
	ENTRY_CHANNEL,
	ENTRY_CONTROLVARIANT,
	ENTRY_FLIPSIGNAL
};

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
	
	int keys_pressed, keys_released;
	bool updateMenu = false;
	
	buildMenu();
	
	do {
		scanKeys();
		
		keys_pressed = keysDown();
		keys_released = keysUp();
		
		if (keys_pressed & KEY_DOWN)
		{
			menuDown();
			
			updateMenu = true;
		}
		
		if (keys_pressed & KEY_UP)
		{
			menuUp();
			
			updateMenu = true;
		}
		
		if (keys_pressed & KEY_LEFT)
		{
			menuLeft();
			
			updateMenu = true;
		}
		
		if (keys_pressed & KEY_RIGHT)
		{
			menuRight();
			
			updateMenu = true;
		}
		
		if (keys_pressed & KEY_START)
		{
			runTankController(deskpetModel, deskpetColor, deskpetChannel, controlVariant, flipSignals);
			
			updateMenu = true;
		}
		
		if (updateMenu)
		{
			buildMenu();
			
			updateMenu = false;
		}
	} while( 1 );
}

void menuUp()
{
	currentMenuIndex = currentMenuIndex - 1;
}

void menuDown()
{
	currentMenuIndex = currentMenuIndex + 1;
}

void menuLeft()
{
	switch (currentMenuEntry) {
		case ENTRY_MODEL:
			deskpetModel = deskpetModel - 1;
			
			controlVariant = 0;
			deskpetChannel = 0;
			deskpetColor = 0;
			
			if (deskpetModel < MODEL_DRIFTBOT)
				deskpetModel = MODEL_DRIFTBOT;
			break;
		case ENTRY_COLOR:
			deskpetColor = deskpetColor - 1;
		
			switch (deskpetModel)
			{
				case MODEL_BATTLETANK:
					if (deskpetColor < BATTLETANK_OLIVE)
						deskpetColor = BATTLETANK_OLIVE;
					break;
				case MODEL_TANKBOT:
					if (deskpetColor < TANKBOT_GREEN)
						deskpetColor = TANKBOT_GREEN;
					break;
				case MODEL_TANKBOTFIRE:
					if (deskpetColor < TANKBOTFIRE_GREEN)
						deskpetColor = TANKBOTFIRE_GREEN;
					break;
				case MODEL_TREKBOT:
					if (deskpetColor < TREKBOT_BLUE)
						deskpetColor = TREKBOT_BLUE;
					break;
				case MODEL_SKITTERBOT:
					if (deskpetColor < SKITTERBOT_BLUE)
						deskpetColor = SKITTERBOT_BLUE;
					break;
			}
			break;
		case ENTRY_CHANNEL:
			deskpetChannel = deskpetChannel - 1;
			
			if (deskpetChannel == 0)
				deskpetChannel = 1;
			break;
		case ENTRY_CONTROLVARIANT:
			controlVariant = controlVariant - 1;
			
			if (controlVariant < CONTROLTYPE_DEFAULT)
				controlVariant = CONTROLTYPE_DEFAULT;
			break;
		case ENTRY_FLIPSIGNAL:
			flipSignals = flipSignals - 1;
			
			if (flipSignals == -1)
				flipSignals = 0;
			break;
	}
}

void menuRight()
{
	switch (currentMenuEntry) {
		case ENTRY_MODEL:
			deskpetModel = deskpetModel + 1;
			
			if (deskpetModel > MODEL_SKITTERBOT)
				deskpetModel = MODEL_SKITTERBOT;
			break;
		case ENTRY_COLOR:
			deskpetColor = deskpetColor + 1;
		
			switch (deskpetModel)
			{
				case MODEL_BATTLETANK:
					if (deskpetColor > BATTLETANK_GREEN)
						deskpetColor = BATTLETANK_GREEN;
					break;
				case MODEL_TANKBOT:
					if (deskpetColor > TANKBOT_WHITE)
						deskpetColor = TANKBOT_WHITE;
					break;
				case MODEL_TANKBOTFIRE:
					if (deskpetColor > TANKBOTFIRE_RED)
						deskpetColor = TANKBOTFIRE_RED;
					break;
				case MODEL_TREKBOT:
					if (deskpetColor > TREKBOT_CLEAR)
						deskpetColor = TREKBOT_CLEAR;
					break;
				case MODEL_SKITTERBOT:
					if (deskpetColor > SKITTERBOT_YELLOW)
						deskpetColor = SKITTERBOT_YELLOW;
					break;
			}
			break;
		case ENTRY_CHANNEL:
			deskpetChannel = deskpetChannel + 1;
			
			if (deskpetChannel == 4)
				deskpetChannel = 3;
			break;
		case ENTRY_CONTROLVARIANT:
			controlVariant = controlVariant + 1;
			
			if (controlVariant > CONTROLTYPE_ALTERNATIVE)
				controlVariant = CONTROLTYPE_ALTERNATIVE;
			break;
		case ENTRY_FLIPSIGNAL:
			flipSignals = flipSignals + 1;
			
			if (flipSignals == 2)
				flipSignals = 1;
			break;
	}
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
	
	if (currentMenuIndex < 0)
		currentMenuIndex = 0;
	
	if (currentMenuIndex == (currentRow - 3))
	{
		currentMenuEntry = ENTRY_MODEL;
		makeMenuEntry(currentRow, "Model:", getModelText(), true);
	}
	else
	{
		makeMenuEntry(currentRow, "Model:", getModelText(), false);
	}
	
	if (isColorAvailable())
	{
		currentRow = currentRow + 1;
		
		if (deskpetColor == 0)
			deskpetColor = 1;
		
		if (currentMenuIndex == (currentRow - 3))
		{
			currentMenuEntry = ENTRY_COLOR;
			makeMenuEntry(currentRow, "Color", getColorText(deskpetColor), true);
		}
		else
		{
			makeMenuEntry(currentRow, "Color", getColorText(deskpetColor), false);
		}
	}
	else
	{	
		deskpetColor = 0;
	}
	
	if (isChannelAvailable())
	{
		currentRow = currentRow + 1;
		
		if (deskpetChannel == 0)
			deskpetChannel = 1;
		
		if (currentMenuIndex == (currentRow - 3))
		{
			currentMenuEntry = ENTRY_CHANNEL;
			switch(deskpetChannel)
			{
				case 1:
					makeMenuEntry(currentRow, "Channel:", "A", true);
					break;
				case 2:
					makeMenuEntry(currentRow, "Channel:", "B", true);
					break;
				case 3:
					makeMenuEntry(currentRow, "Channel:", "C", true);
					break;
			}
		}
		else
		{
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
	}
	else
	{
		deskpetChannel = 0;
	}
	
	if (isAlternativeControlVariantAvailable())
	{
		currentRow = currentRow + 1;
		
		if (controlVariant == 0)
			controlVariant = 1;
		
		if (currentMenuIndex == (currentRow - 3))
		{
			currentMenuEntry = ENTRY_CONTROLVARIANT;
			makeMenuEntry(currentRow, "Controls:", getControlTypeText(), true);
		}
		else
		{
			makeMenuEntry(currentRow, "Controls:", getControlTypeText(), false);
		}
	}
	else
	{
		controlVariant = 0;
	}
	
	currentRow = currentRow + 1;
	
	if (currentMenuIndex == (currentRow - 3))
	{
		currentMenuEntry = ENTRY_FLIPSIGNAL;
		switch(flipSignals)
		{
			case 0:
				makeMenuEntry(currentRow, "Flip Signals:", "off", true);
				break;
			case 1:
				makeMenuEntry(currentRow, "Flip Signals:", "on", true);
		}
	}
	else
	{
		switch(flipSignals)
		{
			case 0:
				makeMenuEntry(currentRow, "Flip Signals:", "off", false);
				break;
			case 1:
				makeMenuEntry(currentRow, "Flip Signals:", "on", false);
		}
	}
	
	if (currentMenuIndex > (currentRow - 3))
	{
		currentMenuIndex = (currentRow - 3);
		buildMenu();
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

bool isAlternativeControlVariantAvailable()
{
	if (deskpetModel != MODEL_SKITTERBOT &
		deskpetModel != MODEL_TREKBOT)
		{
			return true;
		}
		
	return false;
}

bool isChannelAvailable()
{
	if (deskpetModel == MODEL_BATTLETANKV2 |
		deskpetModel == MODEL_TANKBOTV2 |
		deskpetModel == MODEL_CARBOT |
		deskpetModel == MODEL_DRIFTBOT)
		{
			return true;
		}
		
	return false;
}

bool isColorAvailable()
{
	if (deskpetModel == MODEL_BATTLETANK |
		deskpetModel == MODEL_SKITTERBOT |
		deskpetModel == MODEL_TANKBOT |
		deskpetModel == MODEL_TANKBOTFIRE |
		deskpetModel == MODEL_TREKBOT)
		{
			return true;
		}
		
	return false;
}


char *getControlTypeText()
{
	switch (deskpetModel)
	{
		case MODEL_DRIFTBOT:
		case MODEL_CARBOT:
			switch (controlVariant)
			{
				case CONTROLTYPE_DEFAULT:
					return "DPAD + AB-Buttons";
				case CONTROLTYPE_ALTERNATIVE:
					return "DPAD + LR-Buttons";
			}
			break;
		case MODEL_BATTLETANK:
		case MODEL_BATTLETANKV2:
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
		case MODEL_TANKBOTV2:
			switch (controlVariant)
			{
				case CONTROLTYPE_DEFAULT:
					return "DPAD only";
				case CONTROLTYPE_ALTERNATIVE:
					return "DPAD + AB + LR-Buttons";
			}
			break;
	}
	
	return "";
}

char *getModelText()
{
	switch (deskpetModel)
	{
		case MODEL_DRIFTBOT:
			return "Driftbot";
		case MODEL_BATTLETANK:
			return "Battletank";
		case MODEL_BATTLETANKV2:
			return "Battletank V2";
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

char *getColorText()
{	
	switch (deskpetModel)
	{
		case MODEL_BATTLETANK:
			switch (deskpetColor)
			{
				case BATTLETANK_OLIVE:
					return "Olive";
				case BATTLETANK_GREEN:
					return "Green";
			}
			break;
		case MODEL_TANKBOT:
			switch (deskpetColor)
			{
				case TANKBOT_GREEN:
					return "Green";
				case TANKBOT_BLUE:
					return "Blue";
				case TANKBOT_GOLD:
					return "Gold";
				case TANKBOT_ORANGE:
					return "Orange";
				case TANKBOT_GREY:
					return "Grey";
				case TANKBOT_WHITE:
					return "White";
			}
			break;
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOTFIRE_GREEN:
					return "Green";
				case TANKBOTFIRE_BLUE:
					return "Blue";
				case TANKBOTFIRE_GREY:
					return "Grey";
				case TANKBOTFIRE_WHITE:
					return "White";
				case TANKBOTFIRE_RED:
					return "Red";
			}
			break;
		case MODEL_TREKBOT:
			switch (deskpetColor)
			{
				case TREKBOT_BLUE:
					return "Blue";
				case TREKBOT_GOLD:
					return "Gold";
				case TREKBOT_WHITE:
					return "White";
				case TREKBOT_BLACK:
					return "Black";
				case TREKBOT_RED:
					return "Red";
				case TREKBOT_CLEAR:
					return "Clear";
			}
			break;
		case MODEL_SKITTERBOT:
			switch (deskpetColor)
			{
				case SKITTERBOT_BLUE:
					return "Blue";
				case SKITTERBOT_WHITE:
					return "White";
				case SKITTERBOT_RED:
					return "Red";
				case SKITTERBOT_CLEAR:
					return "Clear";
				case SKITTERBOT_YELLOW:
					return "Yellow";
			}
			break;
	}
	
	return "";
};