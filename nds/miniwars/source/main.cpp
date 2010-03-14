// MiniWars - A stylised 2D FPS
// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>
//
// The code is a little bit messy, but it's rather a work-in-progress than a 
// finished product. It will clean itself through the various refactorings

// Includes
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "nds_utils.h"
#include "MovableSprite.h"
#include "Projectile.h"
#include "Crosshair.h"
#include "Ship.h"

void initVideo() {
       	videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_SPRITE);

	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
}

const int max_projectiles = 32;

int get_next_free_projectile(Projectile projectiles[]) {
	// Linear search 
	for (int idx = 0; idx < max_projectiles; idx ++) { 
		if (! projectiles[idx].is_shown) return idx;
	}
	return -1;
}

float in_limit(float value, float min, float max) {
	if (value < min) value = min;
	if (value > max) value = max;
	return value;
}

float sign(float value) {
	// return (value > 0) ? 1 : ( (value == 0) ? 0 : -1 );
	if (value == 0) return 0;
	if (value > 0) return 1;
	return -1;
}

float sqr(float value) {
	return value * value;
}

int debug = 0;

// Function: main()
int main(int argc, char *argv[]) {
	/*  Turn on the 2D graphics core. */
	powerOn(POWER_ALL_2D);

	/*  Configure the VRAM and background control registers. */
	lcdMainOnBottom(); // Place the main screen on the bottom physical screen
	initVideo();
	
	consoleDemoInit();
	
	//by default font will be rendered with color 255
	BG_PALETTE_SUB[255] = RGB15(31,31,31);	

	Projectile projectiles[max_projectiles];

	Crosshair crosshair;
	Ship ship;

	ship.is_shown = true;
	ship.frame = 0;
	ship.dest_frame = 0;
	
	// Infinite loop to keep the program running
	for (unsigned int frame = 0; true; frame++) {
		static touchPosition touch;

		static int last_frame_shoot = frame;

		scanKeys();
		PA_CheckLid();

		// --------
		// Do stuff
		// --------
		int keys = keysHeld();
                if (keys & KEY_TOUCH) {
			touchRead(&touch);
			crosshair.setDestination(
				touch.px,
				touch.py,
				frame, frame
			);
			crosshair.is_shown = true;
		} else {
			crosshair.is_shown = false;
			crosshair.is_shown = true;
		}

		// Movements of the ship
		const int ship_speed = 4;
		static int accuracy = 0;
			
		if (keys & KEY_UP) {
			ship.dest_y = in_limit(ship.dest_y - ship_speed, 0, SCREEN_HEIGHT - ship.getSizeY()); 
			accuracy = 64;
		} else if (keys & KEY_DOWN) {
			ship.dest_y = in_limit(ship.dest_y + ship_speed, 0, SCREEN_HEIGHT - ship.getSizeY()); 
			accuracy = 64;
		}

		if (keys & KEY_LEFT) {
			ship.dest_x = in_limit(ship.dest_x - ship_speed, 0, SCREEN_WIDTH - ship.getSizeX()); 
			accuracy = 64;
		} else if (keys & KEY_RIGHT) {
			ship.dest_x = in_limit(ship.dest_x + ship_speed, 0, SCREEN_WIDTH - ship.getSizeX());
			accuracy = 64;
		}

		if (accuracy > 0) accuracy--;
		
		if ((keys & KEY_L)) {
			// Shoot
			int projectiles_idx = get_next_free_projectile(projectiles);
			if (projectiles_idx == -1) {
				// No projectile left
			} else {
				MovableSprite& projectile = projectiles[projectiles_idx];
				projectile.is_shown = true;
				projectile.x = ship.dest_x;
				projectile.y = ship.dest_x;
				projectile.dest_x = crosshair.dest_x;
				projectile.dest_y = crosshair.dest_y;

				// Adding some randomness
				if (accuracy > 0) {
					float rnd_x = rand() % accuracy - (accuracy / 2);
					float rnd_y = rand() % accuracy - (accuracy / 2);
					
					projectile.dest_x += rnd_x;
					projectile.dest_y += rnd_y;
				}
				
				projectile.frame = frame;

				float distance = sqrt(
					sqr(projectile.dest_x - projectile.x)
					+ sqr(projectile.dest_y - projectile.y)
				);

				projectile.dest_frame = frame + distance / 4;

				last_frame_shoot = frame;
			}
			if(debug) printf("[%d]SHT[%d,%d][%d,%d][%d]\n", frame, 
					int(ship.x), int(ship.y), 
					touch.px, touch.py, 
					projectiles_idx);
		}

		oamSet(&oamMain, //main graphics engine context
                        ship.idx_sprite,        //oam index (0 to 127)  
                        ship.getScreenX(frame), 
			ship.getScreenY(frame),   //x and y pixle location of the sprite
                        0,                    //priority, lower renders last (on top)
                        0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
                        ship.size,
                        SpriteColorFormat_256Color,
                        ship.gfx,                //pointer to the loaded graphics
                        -1,                  //sprite rotation data  
                        false,               //double the size when rotating?
                        !ship.is_shown                //hide the sprite?
		);

		oamSet(&oamMain, //main graphics engine context
			// oam index (0 to 127)  
                        crosshair.idx_sprite,        
			// x and y pixel location of the sprite
                        crosshair.getScreenX(frame), 
			crosshair.getScreenY(frame),   
			// priority, lower renders last (on top)
                        0,                    
			// this is the palette index if multiple palettes 
			// or the alpha value if bmp sprite     
                        0,                    
                        crosshair.size,
                        SpriteColorFormat_256Color,
                        crosshair.gfx, //pointer to the loaded graphics
                        1,                  //sprite rotation data  
                        false,               //double the size when rotating?
                        !crosshair.is_shown  //hide the sprite?
		);

		for (int idx = 0; idx < max_projectiles; idx ++) {
			MovableSprite& projectile = projectiles[idx];
			if (! projectile.is_shown) {
				continue;
			}

			int frames_left = projectile.getFramesLeft(frame);
			if(debug > 1) printf("[%d][%d] frames left [%d]\n", frame, idx, frames_left);
			if (frames_left <= 0) {
				// hide the sprite
				projectile.is_shown = false;
			}

			oamSet(&oamMain, //main graphics engine context
				projectile.idx_sprite,        //oam index (0 to 127)  
				projectile.getScreenX(frame), 
				projectile.getScreenY(frame),   //x and y pixle location of the sprite
				idx,                    //priority, lower renders last (on top)
				0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
				projectile.size,
				SpriteColorFormat_256Color,
				projectile.gfx, //pointer to the loaded graphics
				-1,                  //sprite rotation data  
				false,               //double the size when rotating?
				! projectile.is_shown  //hide the sprite?
			);
		}

		swiWaitForVBlank();

		if (checkReset()) if(debug) printf("[%d] checkReset\n", frame);
		if (keysDown() & KEY_SELECT) { 
			debug = (debug + 1) % 3; 
			printf("[%d] DEBUG %d\n", frame, debug);
	       	}
		oamUpdate(&oamMain);
	}
	
	return 0;
} // End of main()
