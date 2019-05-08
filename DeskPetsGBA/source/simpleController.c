#include <gba.h>
#include <maxmod.h>

#include <stdio.h>
#include <stdlib.h>

#include "soundbank.h"
#include "soundbank_bin.h"

#include "dpVariants.h"
;
mm_sound_effect getForwardSound();
mm_sound_effect getBackwardsSound();
mm_sound_effect getLeftSound();
mm_sound_effect getRightSound();
mm_sound_effect getStopSound();

static bool leave = false;
static bool doNotLeave = true;

static int deskpetModel = 0;
static int deskpetColor = 0;
static int deskpetChannel = 0;
static int controlVariant = 0;
static int flipSignals = 0;

void runSimpleController(int model, int color, int channel, int variant, int signal) {
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
	
	mm_sound_effect Forward = getForwardSound();
	mm_sound_effect Backwards = getBackwardsSound();
	mm_sound_effect Left = getLeftSound();
	mm_sound_effect Right = getRightSound();
	mm_sound_effect Stop = getStopSound();

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	
	iprintf("\x1b[0;5HDeskPets Controller");
	
	iprintf("\x1b[3;0HDPAD = Move around");
	
	if (isFireAvailable())
	{
		iprintf("\x1b[4;0HA = Fire");
	}
	
	if (isBoostAvailable())
	{
		iprintf("\x1b[5;0HB = Activate boost");
		iprintf("\x1b[6;0HSELECT = Switch mode");
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
			if (keys_current & KEY_UP)
			{
				mmEffectEx(&Forward);
			}
			else if (keys_current & KEY_DOWN)
			{
				mmEffectEx(&Backwards);
			}
			else if (keys_current & KEY_LEFT)
			{
				mmEffectEx(&Left);
			}
			else if (keys_current & KEY_RIGHT)
			{
				mmEffectEx(&Right);
			}
			else
			{
				mmEffectEx(&Stop);
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

mm_sound_effect getForwardSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
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
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRFBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRFGREYTANKFLIP } ,			// id
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
		case MODEL_TANKBOTFIRE:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TANKBOTFIRE_BLUE:
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
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_RED: //Red uses Orange audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRFORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRFBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOTFIRE_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_RED: //Red uses Orange audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRFORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRFGREYTANKFLIP } ,			// id
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
		case MODEL_BATTLETANK:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFOLIVEBATTLE } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFGREENBATTLE } ,			// id
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
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFOLIVEBATTLEFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRFGREENBATTLEFLIP } ,			// id
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

mm_sound_effect getBackwardsSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
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
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRSBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRSGREYTANKFLIP } ,			// id
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
		case MODEL_TANKBOTFIRE:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TANKBOTFIRE_BLUE:
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
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_RED: //Red uses Orange audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRSORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRSBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOTFIRE_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_RED: //Red uses Orange audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRSORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRSGREYTANKFLIP } ,			// id
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
		case MODEL_BATTLETANK:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSOLIVEBATTLE } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSGREENBATTLE } ,			// id
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
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSOLIVEBATTLEFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRSGREENBATTLEFLIP } ,			// id
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

mm_sound_effect getLeftSound()
{
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
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
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRBBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRBGREYTANKFLIP } ,			// id
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
		case MODEL_TANKBOTFIRE:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TANKBOTFIRE_BLUE:
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
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_RED: //Red uses Orange audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRBORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRBBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOTFIRE_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_RED: //Red uses Orange audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRBORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LFRBGREYTANKFLIP } ,			// id
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
		case MODEL_BATTLETANK:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBOLIVEBATTLE } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBGREENBATTLE } ,			// id
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
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBOLIVEBATTLEFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LFRBGREENBATTLEFLIP } ,			// id
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

mm_sound_effect getRightSound(){
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
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
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LSRFBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LSRFGREYTANKFLIP } ,			// id
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
		case MODEL_TANKBOTFIRE:
			switch (deskpetColor)
			{
				case TANKBOTFIRE_BLUE:
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
				case TANKBOTFIRE_GREEN:
				{
					mm_sound_effect temp = {
						{ SFX_LSRFGREENTANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
				case TANKBOTFIRE_RED: //Red uses Orange audio files
				{
					mm_sound_effect temp = {
						{ SFX_LSRFORANGETANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
				default: //Grey and White use the same audio files
				{
					mm_sound_effect temp = {
						{ SFX_LSRFBLACKTANK } ,			// id
						(int)(1.0f * (1<<10)),	// rate
						0,		// handle
						255,	// volume
						127,	// panning
					};
					return temp;
				}
			}
			break;
		case MODEL_BATTLETANK:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFOLIVEBATTLE } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFGREENBATTLE } ,			// id
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
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFOLIVEBATTLEFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRFGREENBATTLEFLIP } ,			// id
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

mm_sound_effect getStopSound(){
	switch (deskpetModel)
	{
		case MODEL_TANKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
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
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LSRSBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOT_ORANGE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Gold, Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LSRSGREYTANKFLIP } ,			// id
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
		case MODEL_TANKBOTFIRE:
			if(flipSignals)
			{
				switch (deskpetColor)
				{
					case TANKBOTFIRE_BLUE:
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
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSGREENTANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_RED: //Red uses Orange audio files
					{
						mm_sound_effect temp = {
							{ SFX_LSRSORANGETANK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LSRSBLACKTANK } ,			// id
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
				switch (deskpetColor)
				{
					case TANKBOTFIRE_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSBLUETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSGREENTANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TANKBOTFIRE_RED: //Red uses Orange audio files
					{
						mm_sound_effect temp = {
							{ SFX_LSRSORANGETANKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Grey and White use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LSRSGREYTANKFLIP } ,			// id
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
		case MODEL_BATTLETANK:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSOLIVEBATTLE } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSGREENBATTLE } ,			// id
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
				switch (deskpetColor)
				{
					case BATTLETANK_OLIVE:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSOLIVEBATTLEFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case BATTLETANK_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LSRSGREENBATTLEFLIP } ,			// id
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

