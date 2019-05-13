#include <gba.h>
#include <maxmod.h>

#include <stdio.h>
#include <stdlib.h>

#include "soundbank.h"
#include "soundbank_bin.h"

#include "dpVariants.h"
;
static mm_sound_effect getLFRFSound();
static mm_sound_effect getLFRSSound();
static mm_sound_effect getLFRBSound();
static mm_sound_effect getLSRFSound();
static mm_sound_effect getLSRSSound();
static mm_sound_effect getLSRBSound();
static mm_sound_effect getLBRFSound();
static mm_sound_effect getLBRSSound();
static mm_sound_effect getLBRBSound();
static mm_sound_effect getFireSound();
static mm_sound_effect getBoosterSound();

static bool leave = false;
static bool doNotLeave = true;

static int deskpetModel = 0;
static int deskpetColor = 0;
static int deskpetChannel = 0;
static int controlVariant = 0;
static int flipSignals = 0;

void runCarController(int model, int color, int channel, int variant, int signal) {
	leave = false;
	doNotLeave = true;
	
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
	mm_sound_effect Booster = getBoosterSound();
	
	//prepareAudioFiles();

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	
	iprintf("\x1b[0;5HDeskPets Controller");
	
	if (controlVariant == CONTROLTYPE_DEFAULT)
	{
		iprintf("\x1b[3;0HDPAD = Move left an right");
		
		iprintf("\x1b[4;0HA = Move forward");
		iprintf("\x1b[5;0HB = Move backwards");
		
		if (deskpetModel == MODEL_CARBOT)
		{
			iprintf("\x1b[7;0HL = Activate boost");
			iprintf("\x1b[8;0HR = Honk/Fire");
		}
		else
		{
			iprintf("\x1b[7;0HL = Honk");
			iprintf("\x1b[8;0HR = Drift");
		}
	}
	else if (controlVariant == CONTROLTYPE_ALTERNATIVE)
	{
		iprintf("\x1b[3;0HDPAD = Move left an right");
		
		iprintf("\x1b[4;0HL = Move forward");
		iprintf("\x1b[5;0HR = Move backwards");
		
		if (deskpetModel == MODEL_CARBOT)
		{
			iprintf("\x1b[8;0HB = Honk/Fire");
			iprintf("\x1b[7;0HA = Activate boost");
		}
		else
		{
			iprintf("\x1b[8;0HB = Honk");
			iprintf("\x1b[7;0HA = Drift");
		}
	}
	else
	{
		iprintf("\x1b[3;0HHold your Gameboy like a");
		iprintf("\x1b[4;0HWiimote (DPAD facing up)");
		iprintf("\x1b[6;0HThis mode is only recommended for Gameboy Micro users.");
		
		iprintf("\x1b[9;0HDPAD = Move around");
		iprintf("\x1b[10;0HL = Honk/Fire");
	}
	
	iprintf("\x1b[19;4HSTART = Return to menu");
	
	// sound effect handle (for cancelling it later)
	
	int keys_current = 0;
	int keys_old = 0;
	int keys_released = 0;
	
	do {
		VBlankIntrWait();
		mmFrame();
		
		keys_old = keys_current;
		keys_current = ~REG_KEYINPUT;
		keys_released = REG_KEYINPUT;
		
		if (keys_old != keys_current)
		{
			if (controlVariant == CONTROLTYPE_DEFAULT)
			{
				if (keys_current & KEY_L)
				{
					if (deskpetModel == MODEL_CARBOT)
					{
						mmEffectEx(&Booster);
					}
					else
					{
						//For some reason DeskPets decided to name the file
						//with the Honk command Fire*.wav
						//Thats because the Carbot and Driftbot have 2 RC modes
						//The first one being for normal driving, the other being
						//for battles between cars.
						mmEffectEx(&Fire);
					}
				}
				else if (keys_current & KEY_R)
				{
					if (deskpetModel == MODEL_CARBOT)
					{
						mmEffectEx(&Fire);
					}
					else
					{
						//And they called the Drift command file (possibly)
						//Booster*.wav because this setup makes perfect sense
						mmEffectEx(&Booster);
					}
				}
				else if (keys_current & KEY_A)
				{
					if (keys_current & KEY_LEFT)
					{
						mmEffectEx(&LSRF);
					}
					else if (keys_current & KEY_RIGHT)
					{
						mmEffectEx(&LFRS);
					}
					else
					{
						mmEffectEx(&LFRF);
					}
				}
				else if (keys_current & KEY_B)
				{
					if (keys_current & KEY_LEFT)
					{
						mmEffectEx(&LSRB);
					}
					else if (keys_current & KEY_RIGHT)
					{
						mmEffectEx(&LBRS);
					}
					else
					{
						mmEffectEx(&LBRB);
					}
				}
				else if (keys_current & KEY_LEFT)
				{
					mmEffectEx(&LBRF);
				}
				else if (keys_current & KEY_RIGHT)
				{
					mmEffectEx(&LFRB);
				}
				else
				{
					mmEffectEx(&LSRS);
				}
			}
			else if (controlVariant == CONTROLTYPE_ALTERNATIVE)
			{
				if (keys_current & KEY_A)
				{
					if (deskpetModel == MODEL_CARBOT)
					{
						mmEffectEx(&Booster);
					}
					else
					{
						mmEffectEx(&Fire);
					}
				}
				else if (keys_current & KEY_B)
				{
					if (deskpetModel == MODEL_CARBOT)
					{
						mmEffectEx(&Fire);
					}
					else
					{
						mmEffectEx(&Booster);
					}
				}
				else if (keys_current & KEY_R)
				{
					if (keys_current & KEY_LEFT)
					{
						mmEffectEx(&LSRF);
					}
					else if (keys_current & KEY_RIGHT)
					{
						mmEffectEx(&LFRS);
					}
					else
					{
						mmEffectEx(&LFRF);
					}
				}
				else if (keys_current & KEY_L)
				{
					if (keys_current & KEY_LEFT)
					{
						mmEffectEx(&LSRB);
					}
					else if (keys_current & KEY_RIGHT)
					{
						mmEffectEx(&LBRS);
					}
					else
					{
						mmEffectEx(&LBRB);
					}
				}
				else if (keys_current & KEY_LEFT)
				{
					mmEffectEx(&LBRF);
				}
				else if (keys_current & KEY_RIGHT)
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
				if(keys_current & KEY_L)
				{
					mmEffectEx(&Fire);
				}
				else if (keys_current & KEY_LEFT)
				{
					if (keys_current & KEY_DOWN)
					{
						mmEffectEx(&LSRF);
					}
					else if (keys_current & KEY_UP)
					{
						mmEffectEx(&LFRS);
					}
					else
					{
						mmEffectEx(&LFRF);
					}
				}
				else if (keys_current & KEY_RIGHT)
				{
					if (keys_current & KEY_DOWN)
					{
						mmEffectEx(&LSRB);
					}
					else if (keys_current & KEY_UP)
					{
						mmEffectEx(&LBRS);
					}
					else
					{
						mmEffectEx(&LBRB);
					}
				}
				else if (keys_current & KEY_DOWN)
				{
					mmEffectEx(&LBRF);
				}
				else if (keys_current & KEY_UP)
				{
					mmEffectEx(&LFRB);
				}
				else
				{
					mmEffectEx(&LSRS);
				}
			}
		}
		
		if (keys_current & KEY_START) {
			if (doNotLeave == false)
			{
				mmEffectCancelAll();
				irqDisable (IRQ_VBLANK);
				leave = true;
			}
		}
		
		if (keys_released & KEY_START)
		{
			doNotLeave = false;
		}
	} while( leave == false );
}

static mm_sound_effect getLFRFSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getLFRSSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getLFRBSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getLSRFSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getLSRSSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getLSRBSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LSRBDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getLBRFSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRFDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getLBRSSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRSDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getLBRBSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_LBRBDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getFireSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_FIRECARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_FIRECARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_FIRECARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_FIRECARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_FIRECARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_FIRECARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_FIREDRIFTBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_FIREDRIFTBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_FIREDRIFTBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_FIREDRIFTBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_FIREDRIFTBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_FIREDRIFTBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
	}
}

static mm_sound_effect getBoosterSound()
{
	switch (deskpetModel)
	{
		case MODEL_CARBOT:
			if (flipSignals == 0)
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_BOOSTERCARBOT1 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_BOOSTERCARBOT2 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_BOOSTERCARBOT3 } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			else
			{
				switch (deskpetChannel)
				{
					case 1:
					{
						mm_sound_effect temp = {
							{ SFX_BOOSTERCARBOT1FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 2:
					{
						mm_sound_effect temp = {
							{ SFX_BOOSTERCARBOT2FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case 3:
					{
						mm_sound_effect temp = {
							{ SFX_BOOSTERCARBOT3FLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
				}
			}
			break;
		case MODEL_DRIFTBOT:
			switch (deskpetChannel) //Driftbot does not seem to have flip files for the fire function
			{
				case 1:
				{
					mm_sound_effect temp = {
						{ SFX_BOOSTERDRIFTBOT1 } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
				case 2:
				{
					mm_sound_effect temp = {
						{ SFX_BOOSTERDRIFTBOT2 } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
				case 3:
				{
					mm_sound_effect temp = {
						{ SFX_BOOSTERDRIFTBOT3 } ,			// id
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