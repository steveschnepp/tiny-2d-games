// MiniWars - A stylised 2D FPS
// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>
//
// The code is a little bit messy, but it's rather a work-in-progress than a 
// finished product. It will clean itself through the various refactorings

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

const int max_projectiles = 32;
struct Projectile {
	Projectile() : is_shown(false) {};
	bool is_shown;
	float x;
	float y;
	float dx;
	float dy;
	int frame;
	u16* gfx;
	int idx_sprite;
};

void initSprites(Projectile& ship, Projectile& crosshair_pointed, Projectile& crosshair_current, Projectile projectiles[]) {
	oamInit(&oamMain, SpriteMapping_1D_32, false);
		
	int idx_sprite = 0; // global variable, so don't need to think about it again. But should NEVER go > 127

	// Memory allocation for sprites using oamAllocateGfx. it's like malloc(), but for sprites
	ship.gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_256Color); ship.idx_sprite = idx_sprite++;
	crosshair_current.gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_256Color); crosshair_current.idx_sprite = idx_sprite++;
	crosshair_pointed.gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_256Color); crosshair_pointed.idx_sprite = idx_sprite++;
	for (int idx = 0; idx < max_projectiles; idx++) {
		projectiles[idx].gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_256Color);
		projectiles[idx].idx_sprite = idx_sprite++;
	}

	// Create the main palette
	for (int color = 0; color < 256; color++) {
        	SPRITE_PALETTE[color] = (color << 7) | (1 << 15);
	}
        	
	SPRITE_PALETTE[1] = RGB15(31, 0, 0);
	SPRITE_PALETTE[2] = RGB15(0, 31, 0);
	SPRITE_PALETTE[3] = RGB15(0, 0, 31);
	SPRITE_PALETTE[4] = RGB15(0, 0, 05);

	const int sprite_x = 8;
	const int sprite_y = 8;

	// Fill the sprites with indexed colors
	for (int j = 0; j < sprite_y; j++) {
		for (int i = 0; i < sprite_x/2; i++) {
			int offset = j * sprite_x/2 + i;
                	ship.gfx[offset] = 1 | (1 << 8);
	                crosshair_pointed.gfx[offset] = 3 | (3 << 8);
        	        crosshair_current.gfx[offset] = 4 | (4 << 8);
		}
        }

	const int projectile_x = sprite_x / 2;
	const int projectile_y = sprite_y / 2;
	
	for (int idx = 0; idx < max_projectiles; idx++) {
		for (int j = 0; j < projectile_y; j++) {
			for (int i = 0; i < projectile_x/2; i++) {
				int offset = j * sprite_x/2 + i;
				projectiles[idx].gfx[offset] = 2 | (2 << 8);
			}
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

int sign(int value) {
	// return (value > 0) ? 1 : ( (value == 0) ? 0 : -1 );
	if (value == 0) return 0;
	if (value > 0) return 1;
	return -1;
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

	// Using Projectile for everything, since it's quite generic
	Projectile crosshair_pointed;
	Projectile crosshair_current;
	Projectile ship;

	initSprites(ship, crosshair_pointed, crosshair_current, projectiles);
	ship.is_shown = true;
	
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
			crosshair_pointed.x = touch.px;
			crosshair_pointed.y = touch.py;
			crosshair_pointed.is_shown = true;
		
			// Updating the goal of the current crosshair
			crosshair_current.dx = crosshair_pointed.x;
			crosshair_current.dy = crosshair_pointed.y;
		} else {
			crosshair_pointed.is_shown = false;
		}

		if(debug) printf("c_c.i_s = %s\n", crosshair_current.is_shown ? "true" : "false");
		if ((keys & KEY_TOUCH) && (! crosshair_current.is_shown)) {
			// We didn't show it yet, so just imagine it pops here
			crosshair_current.x = crosshair_current.dx;
			crosshair_current.y = crosshair_current.dy;
			crosshair_current.is_shown = true;
		} else {
			// Slowly move the crosshair pointed to current
			crosshair_current.x += 2 * sign(crosshair_current.dx - crosshair_current.x);
			crosshair_current.y += 2 * sign(crosshair_current.dy - crosshair_current.y);
		}

		// Movements of the ship
		const int ship_size = 8;
		const int ship_speed = 4;
		if (keys & KEY_UP) ship_y = in_limit(ship_y - ship_speed, 0, SCREEN_HEIGHT - ship_size); 
		if (keys & KEY_DOWN) ship_y = in_limit(ship_y + ship_speed, 0, SCREEN_HEIGHT - ship_size); 
		if (keys & KEY_LEFT) ship_x = in_limit(ship_x - ship_speed, 0, SCREEN_WIDTH - ship_size); 
		if (keys & KEY_RIGHT) ship_x = in_limit(ship_x + ship_speed, 0, SCREEN_WIDTH - ship_size);
		
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
				float rnd_x = rand() % aproximation_factor - (aproximation_factor / 2);
				float rnd_y = rand() % aproximation_factor - (aproximation_factor / 2);
				
				projectiles[projectiles_idx].dx = crosshair_current.x + rnd_x;
				projectiles[projectiles_idx].dy = crosshair_current.y + rnd_y;
				
				projectiles[projectiles_idx].frame = frame;

				last_frame_shoot = frame;
			}
			if(debug) printf("[%d]SHT[%d,%d][%d,%d][%d]\n", frame, ship_x, ship_y, touch.px, touch.py, projectiles_idx);
		}


		oamSet(&oamMain, //main graphics engine context
                        ship.idx_sprite,        //oam index (0 to 127)  
                        ship_x, ship_y,   //x and y pixle location of the sprite
                        0,                    //priority, lower renders last (on top)
                        0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
                        SpriteSize_8x8,
                        SpriteColorFormat_256Color,
                        ship.gfx,                //pointer to the loaded graphics
                        -1,                  //sprite rotation data  
                        false,               //double the size when rotating?
                        !ship.is_shown                //hide the sprite?
		);

		oamSet(&oamMain, //main graphics engine context
                	crosshair_current.idx_sprite,        //oam index (0 to 127)  
                        crosshair_current.x, crosshair_current.y,   //x and y pixle location of the sprite
                        0,                    //priority, lower renders last (on top)
                        0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
                        SpriteSize_8x8,
                        SpriteColorFormat_256Color,
                        crosshair_pointed.gfx, //pointer to the loaded graphics
                        -1,                  //sprite rotation data  
                        false,               //double the size when rotating?
                        !crosshair_current.is_shown  //hide the sprite?
		);
		
		oamSet(&oamMain, //main graphics engine context
                        crosshair_pointed.idx_sprite,        //oam index (0 to 127)  
                        crosshair_pointed.x, crosshair_pointed.y,   //x and y pixle location of the sprite
                        0,                    //priority, lower renders last (on top)
                        0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
                        SpriteSize_8x8,
                        SpriteColorFormat_256Color,
                        crosshair_pointed.gfx, //pointer to the loaded graphics
                        -1,                  //sprite rotation data  
                        false,               //double the size when rotating?
                        !crosshair_pointed.is_shown  //hide the sprite?
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
				projectiles[idx].idx_sprite,        //oam index (0 to 127)  
				current_x, current_y,   //x and y pixle location of the sprite
				idx,                    //priority, lower renders last (on top)
				0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite     
				SpriteSize_8x8,
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
