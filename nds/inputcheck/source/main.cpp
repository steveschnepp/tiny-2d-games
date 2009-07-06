// Just print all the inputs
// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>
//
#include <nds.h>
#include <stdio.h>

namespace {
	bool PA_LidClosed() {
		return bool(keysHeld() & KEY_LID);
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
}

int main(int argc, char *argv[]) {
	/* Quickly enable the ANSI console */
	consoleDemoInit();
	
	int nb_a = 0;
	int nb_b = 0;
	int nb_x = 0;
	int nb_y = 0;
	
	int nb_l = 0;
	int nb_r = 0;
	
	int nb_up = 0;
	int nb_down = 0;
	int nb_left = 0;
	int nb_right = 0;
	volatile int frame = 0;
	while(1) {
		frame++;
                scanKeys();

		// Clear screen
		iprintf("\x1b[2J");
		printf("Frame:%d\n", frame); 
	
		{	
		printf("Pressed:\n"); 
                int keys = keysHeld();
		printf("A:%d B:%d X:%d Y:%d L:%d R:%d \n", 
			bool(keys & KEY_A),
			bool(keys & KEY_B),
			bool(keys & KEY_X),
			bool(keys & KEY_Y),
			bool(keys & KEY_L),
			bool(keys & KEY_R)
		);
		printf("^:%d v:%d <:%d >:%d \n", 
			bool(keys & KEY_UP),
			bool(keys & KEY_DOWN),
			bool(keys & KEY_LEFT),
			bool(keys & KEY_RIGHT)
		);
		}
                
		{
		printf("Counter:\n");
		int keys = keysDown();
		nb_a += bool(keys & KEY_A);
		nb_b += bool(keys & KEY_B);
		nb_x += bool(keys & KEY_X);
		nb_y += bool(keys & KEY_Y);
		nb_r += bool(keys & KEY_R);
		nb_l += bool(keys & KEY_L);
		printf("A:%d B:%d X:%d Y:%d L:%d R:%d\n", 
			nb_a,
			nb_b,
			nb_x,
			nb_y,
			nb_l,
			nb_r
		);
		nb_up += bool(keys & KEY_UP);
		nb_down += bool(keys & KEY_DOWN);
		nb_left += bool(keys & KEY_LEFT);
		nb_right += bool(keys & KEY_RIGHT);
		printf("^:%d v:%d <:%d >:%d \n", 
			nb_up,
			nb_down,
			nb_left,
			nb_right
		);
		}
		
		PA_CheckLid();
		swiWaitForVBlank();
	}

	return 0;
}
