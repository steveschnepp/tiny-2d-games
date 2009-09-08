#include "nds_utils.h"

#include <nds.h>

bool PA_LidClosed() {
	return keysHeld() & KEY_LID; 
}

int PA_CheckLid() {
	if (!PA_LidClosed()) {
		return 0;
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
	return 1;
}

void checkReset(int keys) {
	if (keys | KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R) {
		swiSoftReset();
	}
}
