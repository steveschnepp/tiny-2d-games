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

void initVideo() {
    /*
     *  Map VRAM to display a background on the main and sub screens.
     * 
     *  The vramSetMainBanks function takes four arguments, one for each of the
     *  major VRAM banks. We can use it as shorthand for assigning values to
     *  each of the VRAM bank's control registers.
     *
     *  We map banks A and B to main screen background memory. This gives us
     *  256KB, which is a healthy amount for 16-bit graphics.
     *
     *  We map bank C to sub screen background memory.
     *
     *  We map bank D to LCD. This setting is generally used for when we aren't
     *  using a particular bank.
     */
    /*vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000,
                     VRAM_B_MAIN_BG_0x06020000,
                     VRAM_C_SUB_BG_0x06200000,
                     VRAM_D_LCD);
	*/
    vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_SUB_SPRITE);							

	//set main display to render directly from the frame buffer
	videoSetMode(MODE_FB0);
	
    // /*  Set the video mode on the main screen. */
    /* videoSetMode(MODE_5_2D | // Set the graphics mode to Mode 5
                 DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                 DISPLAY_BG3_ACTIVE); //Enable BG3 for display
*/
    /*  Set the video mode on the sub screen. */
    videoSetModeSub(MODE_5_2D | // Set the graphics mode to Mode 5
                    DISPLAY_BG3_ACTIVE); // Enable BG3 for display
}

void initBackgrounds() {
    /*  Set up affine background 3 on main as a 16-bit color background. */
    REG_BG3CNT = BG_BMP16_256x256 |
                 BG_BMP_BASE(0) | // The starting place in memory
                 BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    REG_BG3PA = 1 << 8;
    REG_BG3PB = 0;
    REG_BG3PC = 0;
    REG_BG3PD = 1 << 8;

    /*  Place main screen background 3 at the origin (upper left of the
     *  screen).
     */
    REG_BG3X = 0;
    REG_BG3Y = 0;

    /*  Set up affine background 2 on main as a 16-bit color background. */
    REG_BG2CNT = BG_BMP16_128x128 |
                 BG_BMP_BASE(8) | // The starting place in memory
                 BG_PRIORITY(2);  // A higher priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 1 << 8;

    /*  Place main screen background 2 in an interesting place. */
    REG_BG2X = -(SCREEN_WIDTH / 2 - 32) << 8;
    REG_BG2Y = -32 << 8;

    /*  Set up affine background 3 on the sub screen as a 16-bit color
     *  background.
     */
    REG_BG3CNT_SUB = BG_BMP16_256x256 |
                     BG_BMP_BASE(0) | // The starting place in memory
                     BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the sub screen background 3
     *  to be the identity matrix.
     */
    REG_BG3PA_SUB = 1 << 8;
    REG_BG3PB_SUB = 0;
    REG_BG3PC_SUB = 0;
    REG_BG3PD_SUB = 1 << 8;

    /*
     *  Place main screen background 3 at the origin (upper left of the screen)
     */
    REG_BG3X_SUB = 0;
    REG_BG3Y_SUB = 0;
}

void initGfx() {
	/*  Turn on the 2D graphics core. */
	powerOn(POWER_ALL_2D);
 
	/*  Configure the VRAM and background control registers. */

	// Place the main screen on the bottom physical screen
	lcdMainOnBottom(); 
	initVideo(); 
	initBackgrounds(); 

	consoleDemoInit();
}
