// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>

// We need memset
#include <cstring> 

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

void flip_vram()
{
	if (front == VRAM_A) {
		front = VRAM_B;
		back = VRAM_A;
		videoSetMode(MODE_FB1);			
	} else {
		front = VRAM_A;
		back = VRAM_B;
		videoSetMode(MODE_FB0);			
	}
}

void erase_screen_swi(uint16* screen) {
	const uint16 col = RGB15(0, 0, 0) | BIT(15);
	const uint32 colcol = col | col << 16;
	const int copy_size = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16) / sizeof(uint32);
	swiFastCopy(&colcol, screen, copy_size | COPY_MODE_FILL);
}

void erase_screen_loopmemset(uint16* screen) {
	const int copy_size = SCREEN_WIDTH * SCREEN_HEIGHT;
	const uint16 col = RGB15(0, 0, 0) | BIT(15);
	for (uint16* ptr = screen; ptr < screen + copy_size; ptr++) {
		*ptr = col;
	}
}

void erase_screen_tab(uint16* screen) {
	const int copy_size = SCREEN_WIDTH * SCREEN_HEIGHT;
	const uint16 col = RGB15(0, 0, 0) | BIT(15);
	for (int i = 0; i < copy_size; i++) {
		screen[i] = col;
	}
}

void erase_screen_memset(uint16* screen) {
	// Fill the whole screen buffer with '\0'.
	// Warning, the screen is 16bit whereas '\0' is only 8bit, but it works
	// since it's actually '\0\0' that we want.
	const int copy_size = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16);
	memset(screen, 0, copy_size);
}

void erase_screen(uint16* screen) {
	erase_screen_swi(screen);
}

void Put8bitPixel(int scr_x, int scr_y, unsigned short int color) {
	// Don't put pixel outside screen
	if (scr_x < 0 || scr_x > SCREEN_WIDTH - 1) return;
	if (scr_y < 0 || scr_y > SCREEN_HEIGHT - 1) return;

	// Writing in the framebuffer is a simple memory write.
	back[scr_x + scr_y * SCREEN_WIDTH] = color;
}

void Put8bitLine(int scr_x1, int scr_y1, int scr_x2, int scr_y2, unsigned short int color) {
	if (scr_x1 == scr_x2) {
		// TODO - Special handling of vertical lines
		Put8bitPixel(scr_x1, scr_y1, color);
		Put8bitPixel(scr_x2, scr_y2, color);
		return;
	} 
	
	if (scr_x1 > scr_x2) {
		// Always go from left to right
		// so if right to left : swap src & dst
		int tmp_x = scr_x1;
		scr_x1 = scr_x2;
		scr_x2 = tmp_x;

		int tmp_y = scr_y1;
		scr_y1 = scr_y2;
		scr_y2 = tmp_y;
	}

	// Crude way of writing lines
	for(int x = scr_x1; x <= scr_x2; x++) {
		int y = scr_y1 + (scr_y2 - scr_y1) * (x - scr_x1) / (scr_x2 - scr_x1) ;
		Put8bitPixel(x, y, color); 
	}
}

void Put8bitRect(int scr_x1, int scr_y1, int scr_x2, int scr_y2, unsigned short int color) {
	if (scr_x1 > scr_x2) {
		// Always go from left to right & high to low
		int tmp_x = scr_x1;
		scr_x1 = scr_x2;
		scr_x2 = tmp_x;
	} 
	if (scr_y1 > scr_y1) {
		int tmp_y = scr_y1;
		scr_y1 = scr_y2;
		scr_y2 = tmp_y;
	}

	// Crude way of writing lines
	for(int x = scr_x1; x <= scr_x2; x++) 
	for(int y = scr_y1; y <= scr_y2; y++) {
		Put8bitPixel(x, y, color); 
	}
}
