// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>
#include "nds_utils.h"

uint16* front = VRAM_A;
uint16* back = VRAM_B;

bool PA_LidClosed() {
	return keysHeld() & KEY_LID; 
}

bool PA_CheckLid() {
	if (!PA_LidClosed()) {
		return false;
	}

	// Shutdown everything :p
	powerOff(POWER_ALL_2D); 

	// Wait for the lid to be opened again...
	while(PA_LidClosed()) {
		swiWaitForVBlank();
		scanKeys();
	}

	// Return the power !
	powerOn(POWER_ALL_2D);

	return true;
}

bool checkReset() {
	register int keys = keysHeld();
	if (	true
		&& (keys & KEY_A)
		&& (keys & KEY_B)
		&& (keys & KEY_X)
		&& (keys & KEY_Y)
		&& (keys & KEY_L)
		&& (keys & KEY_R)
	) {
		swiSoftReset();
		return true;
	}
	return false;
}
