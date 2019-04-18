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

bool leave = false;

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
	iprintf("\x1b[0;8HDeskPets");
	iprintf("\x1b[3;0HPress A to go forward");
	iprintf("\x1b[4;0HPress B to go backward");
	
	// sound effect handle (for cancelling it later)
	
	do {

		int current_keys = 0;
		int old_keys = 0;
		
		VBlankIntrWait();
		mmFrame();
	 
		scanKeys();

		old_keys = current_keys;
		current_keys = keysDown();
		
		if (old_keys != current_keys)
		{
			if (controlVariant == CONTROLTYPE_DEFAULT)
			{
				if (current_keys & KEY_UP)
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
				
			}
		}
		
		if (current_keys & KEY_START) {
			mmEffectCancelAll();
			irqDisable (IRQ_VBLANK);
			leave = true;
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