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
static bool doNotLeave = true;

static int deskpetModel = 0;
static int deskpetColor = 0;
static int deskpetChannel = 0;
static int controlVariant = 0;
static int flipSignals = 0;

void runTankController(int model, int color, int channel, int variant, int signal) {
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
	
	//prepareAudioFiles();

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	
	iprintf("\x1b[0;5HDeskPets Controller");
	
	if (controlVariant == CONTROLTYPE_DEFAULT)
	{
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
	}
	else
	{
		iprintf("\x1b[3;0HUp = Move left track forward");
		iprintf("\x1b[4;0HDown = Move left track backwards");
		iprintf("\x1b[6;0HA = Move right track forward");
		iprintf("\x1b[7;0HB = Move right track backwards");
		
		if (isFireAvailable())
		{
			iprintf("\x1b[10;0HR = Fire");
		}
		
		if (isBoostAvailable())
		{
			iprintf("\x1b[11;0HL = Activate boost");
			iprintf("\x1b[12;0HSELECT = Switch mode");
		}
	}
	
	iprintf("\x1b[18;0HSTART = Return to menu.");
	
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
				if((keys_current & KEY_A) & isFireAvailable())
				{
					mmEffectEx(&Fire);
				}
				else if (keys_current & KEY_UP)
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
				else if (keys_current & KEY_DOWN)
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
				int leftTrack = 0;
				int rightTrack = 0;
				
				if((keys_current & KEY_R) & isFireAvailable())
				{
					mmEffectEx(&Fire);
					continue;
				}
				
				if (keys_current & KEY_DOWN)
				{
					leftTrack = -1;
				}
				
				if (keys_current & KEY_UP)
				{
					leftTrack = 1;
				}
				
				if (keys_current & KEY_B)
				{
					rightTrack = -1;
				}
				
				if (keys_current & KEY_A)
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

mm_sound_effect getLFRFSound()
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
		
	}
}

mm_sound_effect getLFRSSound()
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
		
	}
}

mm_sound_effect getLFRBSound()
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
	}
}

mm_sound_effect getLSRFSound(){
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
		
	}
}

mm_sound_effect getLSRSSound(){
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
		
	}
}

mm_sound_effect getLSRBSound()
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
							{ SFX_LSRBBLUETANK } ,			// id
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
							{ SFX_LSRBGREENTANK } ,			// id
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
							{ SFX_LSRBORANGETANK } ,			// id
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
							{ SFX_LSRBBLACKTANK } ,			// id
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
							{ SFX_LSRBBLUETANKFLIP } ,			// id
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
							{ SFX_LSRBGREENTANKFLIP } ,			// id
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
							{ SFX_LSRBORANGETANKFLIP } ,			// id
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
							{ SFX_LSRBGREYTANKFLIP } ,			// id
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
							{ SFX_LSRBBLUETANK } ,			// id
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
							{ SFX_LSRBGREENTANK } ,			// id
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
							{ SFX_LSRBORANGETANK } ,			// id
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
							{ SFX_LSRBBLACKTANK } ,			// id
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
							{ SFX_LSRBBLUETANKFLIP } ,			// id
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
							{ SFX_LSRBGREENTANKFLIP } ,			// id
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
							{ SFX_LSRBORANGETANKFLIP } ,			// id
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
							{ SFX_LSRBGREYTANKFLIP } ,			// id
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

mm_sound_effect getLBRFSound(){
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
							{ SFX_LBRFBLUETANK } ,			// id
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
							{ SFX_LBRFGREENTANK } ,			// id
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
							{ SFX_LBRFORANGETANK } ,			// id
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
							{ SFX_LBRFBLACKTANK } ,			// id
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
							{ SFX_LBRFBLUETANKFLIP } ,			// id
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
							{ SFX_LBRFGREENTANKFLIP } ,			// id
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
							{ SFX_LBRFORANGETANKFLIP } ,			// id
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
							{ SFX_LBRFGREYTANKFLIP } ,			// id
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
							{ SFX_LBRFBLUETANK } ,			// id
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
							{ SFX_LBRFGREENTANK } ,			// id
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
							{ SFX_LBRFORANGETANK } ,			// id
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
							{ SFX_LBRFBLACKTANK } ,			// id
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
							{ SFX_LBRFBLUETANKFLIP } ,			// id
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
							{ SFX_LBRFGREENTANKFLIP } ,			// id
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
							{ SFX_LBRFORANGETANKFLIP } ,			// id
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
							{ SFX_LBRFGREYTANKFLIP } ,			// id
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

mm_sound_effect getLBRSSound()
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
							{ SFX_LBRSBLUETANK } ,			// id
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
							{ SFX_LBRSGREENTANK } ,			// id
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
							{ SFX_LBRSORANGETANK } ,			// id
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
							{ SFX_LBRSBLACKTANK } ,			// id
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
							{ SFX_LBRSBLUETANKFLIP } ,			// id
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
							{ SFX_LBRSGREENTANKFLIP } ,			// id
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
							{ SFX_LBRSORANGETANKFLIP } ,			// id
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
							{ SFX_LBRSGREYTANKFLIP } ,			// id
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
							{ SFX_LBRSBLUETANK } ,			// id
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
							{ SFX_LBRSGREENTANK } ,			// id
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
							{ SFX_LBRSORANGETANK } ,			// id
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
							{ SFX_LBRSBLACKTANK } ,			// id
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
							{ SFX_LBRSBLUETANKFLIP } ,			// id
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
							{ SFX_LBRSGREENTANKFLIP } ,			// id
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
							{ SFX_LBRSORANGETANKFLIP } ,			// id
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
							{ SFX_LBRSGREYTANKFLIP } ,			// id
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

mm_sound_effect getLBRBSound()
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
							{ SFX_LBRBBLUETANK } ,			// id
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
							{ SFX_LBRBGREENTANK } ,			// id
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
							{ SFX_LBRBORANGETANK } ,			// id
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
							{ SFX_LBRBBLACKTANK } ,			// id
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
							{ SFX_LBRBBLUETANKFLIP } ,			// id
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
							{ SFX_LBRBGREENTANKFLIP } ,			// id
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
							{ SFX_LBRBORANGETANKFLIP } ,			// id
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
							{ SFX_LBRBGREYTANKFLIP } ,			// id
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
							{ SFX_LBRBBLUETANK } ,			// id
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
							{ SFX_LBRBGREENTANK } ,			// id
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
							{ SFX_LBRBORANGETANK } ,			// id
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
							{ SFX_LBRBBLACKTANK } ,			// id
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
							{ SFX_LBRBBLUETANKFLIP } ,			// id
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
							{ SFX_LBRBGREENTANKFLIP } ,			// id
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
							{ SFX_LBRBORANGETANKFLIP } ,			// id
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
							{ SFX_LBRBGREYTANKFLIP } ,			// id
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

mm_sound_effect getFireSound(){
	switch (deskpetModel)
	{
		case MODEL_TANKBOTFIRE:
			if (flipSignals == 0)
			{
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
					case TANKBOTFIRE_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_FIREGREENTANK } ,			// id
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
							{ SFX_FIREORANGETANK } ,			// id
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
							{ SFX_FIREBLACKTANK } ,			// id
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
							{ SFX_FIREBLUETANKFLIP } ,			// id
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
							{ SFX_FIREGREENTANKFLIP } ,			// id
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
							{ SFX_FIREORANGETANKFLIP } ,			// id
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
							{ SFX_FIREGREYTANKFLIP } ,			// id
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

bool isFireAvailable()
{	
	return deskpetModel != MODEL_TANKBOT;
}

bool isBoostAvailable()
{
	return deskpetModel == MODEL_BATTLETANK | deskpetModel == MODEL_BATTLETANKV2;
}