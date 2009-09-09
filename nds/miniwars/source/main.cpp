// MiniWars - A stylised 2D FPS
// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>

// Includes
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>

#include "nds_utils.h"

void initVideo() {
       	videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_SPRITE);

	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
}

u16* ship;
u16* crossHair;

const int max_projectiles = 32;
struct Projectile {
	bool is_shown;
	int x;
	int y;
	int dx;
	int dy;
	int frame;
	u16* gfx;
};

void initSprite(Projectile projectiles[]) {
	oamInit(&oamMain, SpriteMapping_1D_32, false);
	// oamAllocateGfx is like malloc(), but for sprites
	ship = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
	crossHair = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);

	for (int idx = 0; idx < max_projectiles; idx++) {
		projectiles[idx].gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
	}

	// Create the main palette
	for (int color = 0; color < 256; color++) {
        	SPRITE_PALETTE[color] = (color << 7) | (1 << 15);
	}
        	
	SPRITE_PALETTE[1] = RGB15(31, 0, 0);
	SPRITE_PALETTE[2] = RGB15(0, 31, 0);
	SPRITE_PALETTE[3] = RGB15(0, 0, 31);

	// Fill the sprites with indexed colors
	for (int i = 0; i < 16 * 16 / 2; i++) {
                ship[i] = 1 | (1 << 8);
                crossHair[i] = 3 | (1 << 8);
        }
	
	for (int idx = 0; idx < max_projectiles; idx++) {
		for (int i = 0; i < 16 * 16; i++) {
			projectiles[idx].gfx[i] = 2 | (1 << 8);
		}
	}
}

int get_next_free_projectile(Projectile projectiles[]) {
	// Linear search 
	for (int idx = 0; idx < max_projectiles; idx ++) { 
		if (! projectiles[idx].is_shown) return idx;
	}
	return -1;
}

int in_limit(int value, int min, int max) {
	if (value < min) value = min;
	if (value > max) value = max;
	return value;
}

bool debug = false;

// Function: main()
int main(int argc, char *argv[]) {
	/*  Turn on the 2D graphics core. */
	powerOn(POWER_ALL_2D);

	/*  Configure the VRAM and background control registers. */
	lcdMainOnBottom(); // Place the main screen on the bottom physical screen
	initVideo();
	
	consoleDemoInit();
	
	BG_PALETTE_SUB[255] = RGB15(31,31,31);	//by default font will be rendered with color 255

	Projectile projectiles[max_projectiles];
	initSprite(projectiles);
	
	// Infinite loop to keep the program running
	for (int frame = 0; true; frame++) {
		static touchPosition touch;
		static int ship_x = 64;
		static int ship_y = 64;

		static int last_frame_shoot = frame;

		scanKeys();
		PA_CheckLid();

		// --------
		// Do stuff
		// --------
		int keys = keysHeld();
                if (keys & KEY_TOUCH) {
			touchRead(&touch);
		}

		// Movements of the ship
		const int ship_speed = 4;
		if (keys & KEY_UP) ship_y = in_limit(ship_y + ship_speed, 0, SCREEN_HEIGHT); 
		if (keys & KEY_DOWN) ship_y = in_limit(ship_y - ship_speed, 0, SCREEN_HEIGHT); 
		if (keys & KEY_LEFT) ship_x = in_limit(ship_x - ship_speed, 0, SCREEN_WIDTH); 
		if (keys & KEY_RIGHT) ship_x = in_limit(ship_x + ship_speed, 0, SCREEN_WIDTH);
		
		if (keys & KEY_L && (frame - last_frame_shoot) > 3) {
			// Shoot
			int projectiles_idx = get_next_free_projectile(projectiles);
			if (projectiles_idx == -1) {
				// No projectile left
			} else {
				projectiles[projectiles_idx].is_shown = true;
				projectiles[projectiles_idx].x = ship_x;
				projectiles[projectiles_idx].y = ship_y;

				// Adding some randomness
				const int aproximation_factor = 64;
				int rnd_x = rand() % aproximation_factor - (aproximation_factor / 2);
				int rnd_y = rand() % aproximation_factor - (aproximation_factor / 2);
				
				projectiles[projectiles_idx].dx = touch.px + rnd_x;
				projectiles[projectiles_idx].dy = touch.py + rnd_y;
				
				projectiles[projectiles_idx].frame = frame;

				last_frame_shoot = frame;
			}
			if(debug) printf("[%d]SHT[%d,%d][%d,%d][%d]\n", frame, ship_x, ship_y, touch.px, touch.py, projectiles_idx);
		}

		oamSet(&oamMain, //main graphics engine context
                        0,        //oam index (0 to 127)  
                        ship_x, ship_y,   //x and y pixle location of the sprite
                        0,                    //priority, lower renders last (on top)
                        0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
                        SpriteSize_16x16,
                        SpriteColorFormat_256Color,
                        ship,                //pointer to the loaded graphics
                        -1,                  //sprite rotation data  
                        false,               //double the size when rotating?
                        false                //hide the sprite?
		);

		oamSet(&oamMain, //main graphics engine context
                        1,        //oam index (0 to 127)  
                        touch.px, touch.py,   //x and y pixle location of the sprite
                        0,                    //priority, lower renders last (on top)
                        0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
                        SpriteSize_16x16,
                        SpriteColorFormat_256Color,
                        crossHair,                 //pointer to the loaded graphics
                        -1,                  //sprite rotation data  
                        false,               //double the size when rotating?
                        !(keys & KEY_TOUCH)  //hide the sprite?
		);

		const int nb_frames_to_target = 33;	
		for (int idx = 0; idx < max_projectiles; idx ++) {
			if (! projectiles[idx].is_shown) {
				continue;
			}

			int frames_left = (projectiles[idx].frame + nb_frames_to_target) - frame;
			if(debug) printf("[%d][%d] frames left [%d]\n", frame, idx, frames_left);
			if (frames_left <= 0) {
				// hide the sprite
				projectiles[idx].is_shown = false;
			}

			int current_x = projectiles[idx].dx - (projectiles[idx].dx - projectiles[idx].x) * (frames_left) / nb_frames_to_target;
			int current_y = projectiles[idx].dy - (projectiles[idx].dy - projectiles[idx].y) * (frames_left) / nb_frames_to_target;

			oamSet(&oamMain, //main graphics engine context
				2 + idx,        //oam index (0 to 127)  
				current_x, current_y,   //x and y pixle location of the sprite
				idx,                    //priority, lower renders last (on top)
				0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				projectiles[idx].gfx, //pointer to the loaded graphics
				-1,                  //sprite rotation data  
				false,               //double the size when rotating?
				! projectiles[idx].is_shown  //hide the sprite?
			);
		}

		swiWaitForVBlank();

		if (checkReset()) if(debug) printf("[%d] checkReset\n", frame);
		if (keysDown() & KEY_SELECT) { debug = !debug; printf("[%d] DEBUG %s\n", frame, debug ? "ON" : "OFF"); }
		oamUpdate(&oamMain);
	}
	
	return 0;
} // End of main()
