#include <gba.h>
#include <maxmod.h>

#include <stdio.h>
#include <stdlib.h>

#include "soundbank.h"
#include "soundbank_bin.h"

bool leave = false;

void runTankController() {
	leave = false;
	
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

	mm_sound_effect forward = {
		{ SFX_LFRFBLUETANK } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		127,	// panning
	};

	mm_sound_effect stop = {
		{ SFX_LSRSBLUETANK } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		127,	// panning
	};

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	iprintf("\x1b[0;8HDeskPets");
	iprintf("\x1b[3;0HDrücke A zum beschleunigen");
	iprintf("\x1b[4;0HDrücke B zum anhalten");
	
	// sound effect handle (for cancelling it later)
	mm_sfxhand fw = 0;
	mm_sfxhand st = 0;

	do {

		int keys_pressed, keys_released;
		
		VBlankIntrWait();
		mmFrame();
	 
		scanKeys();

		keys_pressed = keysDown();
		keys_released = keysUp();

		// Play looping ambulance sound effect out of left speaker if A button is pressed
		if ( keys_pressed & KEY_A ) {
			fw = mmEffectEx(&forward);
		}

		// stop ambulance sound when A button is released
		if ( keys_released & KEY_A ) {
			mmEffectCancel(fw);
		}

		// Play explosion sound effect out of right speaker if B button is pressed
		if ( keys_pressed & KEY_B ) {
			st = mmEffectEx(&stop);
		}
		
		if ( keys_released & KEY_B ) {
			mmEffectCancel(st);
		}
		
		if (keys_pressed & KEY_START) {
			irqDisable (IRQ_VBLANK);
			leave = true;
		}
	} while( leave == false );
}

void prepareAudioFiles()
{
	
}
