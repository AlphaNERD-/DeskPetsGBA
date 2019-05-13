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
	
	if (controlVariant == CONTROLTYPE_DEFAULT)
	{
		iprintf("\x1b[3;0HDPAD = Move around");
	}
	else
	{
		iprintf("\x1b[3;0HHold your Gameboy like a");
		iprintf("\x1b[4;0HWiimote (DPAD facing up)");
		iprintf("\x1b[6;0HThis mode is only recommended for Gameboy Micro users.");
		
		iprintf("\x1b[9;0HDPAD = Move around");
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
			else
			{
				if (keys_current & KEY_LEFT)
				{
					mmEffectEx(&Forward);
				}
				else if (keys_current & KEY_RIGHT)
				{
					mmEffectEx(&Backwards);
				}
				else if (keys_current & KEY_DOWN)
				{
					mmEffectEx(&Left);
				}
				else if (keys_current & KEY_UP)
				{
					mmEffectEx(&Right);
				}
				else
				{
					mmEffectEx(&Stop);
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

mm_sound_effect getForwardSound()
{
	switch (deskpetModel)
	{
		case MODEL_TREKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_FWDREDTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_FWDBLACKTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_FWDCLEARTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue, White and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_FWDBLUETREK } ,			// id
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
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_FWDREDTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_FWDBLACKTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_FWDCLEARTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_WHITE:
					{
						mm_sound_effect temp = {
							{ SFX_FWDWHITETREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_FWDBLUETREKFLIP } ,			// id
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
		case MODEL_SKITTERBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_FWDBLUEBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_FWDGREENBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_FWDREDBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_FWDWHITEBUG } ,			// id
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
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_FWDBLUEBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_FWDGREENBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_FWDREDBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_FWDWHITEBUGFLIP } ,			// id
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
		case MODEL_TREKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_BWDREDTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_BWDBLACKTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_BWDCLEARTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue, White and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_BWDBLUETREK } ,			// id
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
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_BWDREDTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_BWDBLACKTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_BWDCLEARTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_WHITE:
					{
						mm_sound_effect temp = {
							{ SFX_BWDWHITETREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_BWDBLUETREKFLIP } ,			// id
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
		case MODEL_SKITTERBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_BWDBLUEBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_BWDGREENBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_BWDREDBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_BWDWHITEBUG } ,			// id
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
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_BWDBLUEBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_BWDGREENBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_BWDREDBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_BWDWHITEBUGFLIP } ,			// id
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
		case MODEL_TREKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTREDTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTBLACKTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTCLEARTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue, White and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LEFTBLUETREK } ,			// id
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
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTREDTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTBLACKTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTCLEARTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_WHITE:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTWHITETREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LEFTBLUETREKFLIP } ,			// id
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
		case MODEL_SKITTERBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTBLUEBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTGREENBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTREDBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LEFTWHITEBUG } ,			// id
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
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTBLUEBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTGREENBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_LEFTREDBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_LEFTWHITEBUGFLIP } ,			// id
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

mm_sound_effect getRightSound()
{
	switch (deskpetModel)
	{
		case MODEL_TREKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTREDTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTBLACKTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTCLEARTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue, White and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTBLUETREK } ,			// id
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
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTREDTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTBLACKTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTCLEARTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_WHITE:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTWHITETREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTBLUETREKFLIP } ,			// id
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
		case MODEL_SKITTERBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTBLUEBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTGREENBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTREDBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTWHITEBUG } ,			// id
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
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTBLUEBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTGREENBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTREDBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_RIGHTWHITEBUGFLIP } ,			// id
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

mm_sound_effect getStopSound()
{
	switch (deskpetModel)
	{
		case MODEL_TREKBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_STOPREDTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_STOPBLACKTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_STOPCLEARTREK } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue, White and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_STOPBLUETREK } ,			// id
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
					case TREKBOT_YELLOW:
					case TREKBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_STOPREDTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_BLACK:
					{
						mm_sound_effect temp = {
							{ SFX_STOPBLACKTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_CLEAR:
					{
						mm_sound_effect temp = {
							{ SFX_STOPCLEARTREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case TREKBOT_WHITE:
					{
						mm_sound_effect temp = {
							{ SFX_STOPWHITETREKFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Blue and Gold use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_STOPBLUETREKFLIP } ,			// id
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
		case MODEL_SKITTERBOT:
			if (flipSignals == 0)
			{
				switch (deskpetColor)
				{
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_STOPBLUEBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_STOPGREENBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_STOPREDBUG } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_STOPWHITEBUG } ,			// id
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
					case SKITTERBOT_BLUE:
					{
						mm_sound_effect temp = {
							{ SFX_STOPBLUEBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_GREEN:
					{
						mm_sound_effect temp = {
							{ SFX_STOPGREENBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					case SKITTERBOT_RED:
					{
						mm_sound_effect temp = {
							{ SFX_STOPREDBUGFLIP } ,			// id
							(int)(1.0f * (1<<10)),	// rate
							0,		// handle
							255,	// volume
							127,	// panning
						};
						return temp;
					}
					default: //Yellow and clear use the same audio files
					{
						mm_sound_effect temp = {
							{ SFX_STOPWHITEBUGFLIP } ,			// id
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

