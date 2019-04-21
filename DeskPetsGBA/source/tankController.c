#include <gba.h>
#include <maxmod.h>

#include <stdio.h>
#include <stdlib.h>

#include "soundbank.h"
#include "soundbank_bin.h"

#include "dpVariants.h"
;
mm_sound_effect getLFRFSound();
mm_sound_effect getLFRSSound();
mm_sound_effect getLFRBSound();
mm_sound_effect getLSRFSound();
mm_sound_effect getLSRSSound();
mm_sound_effect getLSRBSound();
mm_sound_effect getLBRFSound();
mm_sound_effect getLBRSSound();
mm_sound_effect getLBRBSound();
mm_sound_effect getFireSound();

bool isFireAvailable();
bool isBoostAvailable();

static bool leave = false;

static int deskpetModel = 0;
static int deskpetColor = 0;
static int deskpetChannel = 0;
static int controlVariant = 0;
static int flipSignals = 0;

void runTankController(int model, int color, int channel, int variant, int signal) {
	leave = false;
	
	deskpetModel = model;
	deskpetColor = color;
	deskpetChannel = channel;
	controlVariant = variant;
	flipSignals = signal;
	
	irqInit();

	// Maxmod requires the vblank interrupt to reset sound DMA.
	// Link the VBlank interrupt to mmVBlank, and enable it. 
	irqSet( IRQ_VBLANK, mmVBlank );
	irqEnable( IRQ_VBLANK );

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// initialise maxmod with soundbank and 8 channels
    mmInitDefault( (mm_addr)soundbank_bin, 8 );
	
	mm_sound_effect LFRF = getLFRFSound();
	mm_sound_effect LFRS = getLFRSSound();
	mm_sound_effect LFRB = getLFRBSound();
	mm_sound_effect LSRF = getLSRFSound();
	mm_sound_effect LSRS = getLSRSSound();
	mm_sound_effect LSRB = getLSRBSound();
	mm_sound_effect LBRF = getLBRFSound();
	mm_sound_effect LBRS = getLBRSSound();
	mm_sound_effect LBRB = getLBRBSound();
	mm_sound_effect Fire = getFireSound();
	
	//prepareAudioFiles();

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	
	iprintf("\x1b[0;5HDeskPets Controller");
	
	if (controlVariant == CONTROLTYPE_DEFAULT)
	{
		iprintf("\x1b[3;0HUse the DPAD to move around.");
		
		if (isFireAvailable())
		{
			iprintf("\x1b[4;0HPress A to fire.");
		}
		
		if (isBoostAvailable())
		{
			iprintf("\x1b[5;0HPress B to boost.");
			iprintf("\x1b[6;0HPress SELECT to switch between modes.");
		}
	}
	else
	{
		iprintf("\x1b[3;0HPress Up to move the left track forward.");
		iprintf("\x1b[4;0HPress Down to move the left track backwards.");
		iprintf("\x1b[5;0HPress A to move the right track forward.");
		iprintf("\x1b[6;0HPress B to move the left track backwards.");
		
		if (isFireAvailable())
		{
			iprintf("\x1b[7;0HPress R to fire.");
		}
		
		if (isBoostAvailable())
		{
			iprintf("\x1b[8;0HPress L to boost.");
			iprintf("\x1b[9;0HPress SELECT to switch between modes.");
		}
	}
	
	iprintf("\x1b[18;0HPress START and SELECT to return to the menu.");
	
	// sound effect handle (for cancelling it later)
	
	int current_keys = 0;
	int old_keys = 0;
	
	do {
		VBlankIntrWait();
		mmFrame();
		
		old_keys = current_keys;
		current_keys = ~REG_KEYINPUT;
		
		if (old_keys != current_keys)
		{
			if (controlVariant == CONTROLTYPE_DEFAULT)
			{
				if((current_keys & KEY_A) & isFireAvailable())
				{
					mmEffectEx(&Fire);
				}
				else if (current_keys & KEY_UP)
				{
					if (current_keys & KEY_LEFT)
					{
						mmEffectEx(&LSRF);
					}
					else if (current_keys & KEY_RIGHT)
					{
						mmEffectEx(&LFRS);
					}
					else
					{
						mmEffectEx(&LFRF);
					}
				}
				else if (current_keys & KEY_DOWN)
				{
					if (current_keys & KEY_LEFT)
					{
						mmEffectEx(&LSRB);
					}
					else if (current_keys & KEY_RIGHT)
					{
						mmEffectEx(&LBRS);
					}
					else
					{
						mmEffectEx(&LBRB);
					}
				}
				else if (current_keys & KEY_LEFT)
				{
					mmEffectEx(&LBRF);
				}
				else if (current_keys & KEY_RIGHT)
				{
					mmEffectEx(&LFRB);
				}
				else
				{
					mmEffectEx(&LSRS);
				}
			}
			else
			{
				int leftTrack = 0;
				int rightTrack = 0;
				
				if((current_keys & KEY_R) & isFireAvailable())
				{
					mmEffectEx(&Fire);
					continue;
				}
				
				if (current_keys & KEY_DOWN)
				{
					leftTrack = -1;
				}
				
				if (current_keys & KEY_UP)
				{
					leftTrack = 1;
				}
				
				if (current_keys & KEY_B)
				{
					rightTrack = -1;
				}
				
				if (current_keys & KEY_A)
				{
					rightTrack = 1;
				}
				
				switch (leftTrack)
				{
					case -1:
						switch (rightTrack)
						{
							case -1:
								mmEffectEx(&LBRB);
								break;
							case 0:
								mmEffectEx(&LBRS);
								break;
							case 1:
								mmEffectEx(&LBRF);
						}
						break;
					case 0:
						switch (rightTrack)
						{
							case -1:
								mmEffectEx(&LSRB);
								break;
							case 0:
								mmEffectEx(&LSRS);
								break;
							case 1:
								mmEffectEx(&LSRF);
						}
						break;
					case 1:
						switch (rightTrack)
						{
							case -1:
								mmEffectEx(&LFRB);
								break;
							case 0:
								mmEffectEx(&LFRS);
								break;
							case 1:
								mmEffectEx(&LFRF);
						}
						break;
				}
			}
		}
		
		if (current_keys & KEY_START) {
			if (current_keys & KEY_SELECT)
			{
				mmEffectCancelAll();
				irqDisable (IRQ_VBLANK);
				leave = true;
			}
		}
	} while( leave == false );
}

mm_sound_effect getLFRFSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LFRFBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getLFRSSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LFRSBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getLFRBSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LFRBBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getLSRFSound(){
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LSRFBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getLSRSSound(){
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LSRSBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getLSRBSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LSRBBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getLBRFSound(){
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LBRFBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getLBRSSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LBRSBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getLBRBSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOT_BLUE: //and TANKBOTFIRE_BLUE
				{
					mm_sound_effect temp = {
						{ SFX_LBRBBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

mm_sound_effect getFireSound(){
	switch (deskpetModel)
	{
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOTFIRE_BLUE:
				{
					mm_sound_effect temp = {
						{ SFX_FIREBLUETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		
	}
}

bool isFireAvailable()
{	
	return deskpetModel != MODEL_TANKBOT;
}

bool isBoostAvailable()
{
	return deskpetModel == MODEL_BATTLETANK | deskpetModel == MODEL_BATTLETANKV2;
}