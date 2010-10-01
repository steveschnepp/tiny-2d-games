// MiniWars - A stylised 2D FPS
// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
//
// The code is a little bit messy, but it's rather a work-in-progress than a 
// finished product. It will clean itself through the various refactorings

// Includes
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>

#include <ext/slist>
#include <list>
#include <vector>

#include "nds_utils.h"
#include "game_world.h"

#include "MovableSprite.h"
#include "Ship.h"
#include "Crosshair.h"
#include "Particle.h"
#include "Projectile.h"
#include "Monster.h"

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

typedef std::list<MovableSprite*> sprite_list;
typedef std::list<Monster*> monster_list;

static sprite_list sprites;
static monster_list monsters;

int draw_all_sprites() {
	int removed_sprites = 0;
	// Draws every Sprite on the back screen
	for(sprite_list::iterator i = sprites.begin(); i != sprites.end(); ++i) {
		MovableSprite* s = *i;
		if(s->isExpired()) {
			// Remove the sprite
			i = sprites.erase(i);
			removed_sprites ++;
		 	delete(s);
		} else if (s->isShown()) {
			s->draw();
		}
	}
	
	return removed_sprites;
}

void update_monsters() {
	for(monster_list::iterator i = monsters.begin(); i != monsters.end(); ++i) {
		Monster* s = *i;
		s->update();
		
		if(s->isDead()) {
			// Remove the sprite
			i = monsters.erase(i);
		//	delete(s);
		}
	}
}

void draw_cpu_usage(int usage) {
	const unsigned int color_busy = RGB15(0, 15, 0);
	const unsigned int color_free = RGB15(0, 7, 0);
	// Busy
	for (int i = 0; i < usage; i++) { 
		Put8bitPixel(i, 0, color_busy);
	}

	// Free
	for (int i = usage; i < 100; i++) { 
		Put8bitPixel(i, 0, color_free);
	}
}

// Function: main()
int main(int argc, char *argv[]) {
	// Initialize the gameworld. 
	game_world_init();

	/*  Turn on the 2D graphics core. */
	powerOn(POWER_ALL_2D);
 
	/*  Configure the VRAM and background control registers. */

	// Place the main screen on the bottom physical screen
	lcdMainOnBottom(); 
	initVideo(); 
	initBackgrounds(); 

	consoleDemoInit();

	Ship ship = Ship();
	sprites.push_back(& ship);

	ship.setPosition(64, 64);
	ship.setShown(true);

	Crosshair crosshair = Crosshair();
	crosshair.setPosition(64, 64);
	sprites.push_back(& crosshair);

	int stop_erase = 0;
	int start_erase = 0;
	int start_frame_ms = current_ms;
	bool debug = true;
	
	// Infinite loop to keep the program running
	while (1) {
		++current_frame; // XXX - Should use interrupts...
		start_frame_ms = current_ms;
		
		static touchPosition touch;
		touchRead(&touch);
		
		scanKeys();
		PA_CheckLid();

		// checkReset(held);

		// --------
		// Do stuff
		// --------
		int held = keysHeld();
		int down = keysDown();
		if (held & KEY_UP) {
			ship.moveTo(0, -10, 1);
		} else if (held & KEY_DOWN) {
			ship.moveTo(0, 10, 1);
		}
		
		if (held & KEY_RIGHT) {
			ship.moveTo(10, 0, 1);
		} else if (held & KEY_LEFT) {
			ship.moveTo(-10, 0, 1);
		}
		
		if (down & KEY_A) {
			Monster* monster = new Monster(0, 0, ship); 
			sprites.push_back(monster);
			monsters.push_back(monster);
		} 
		
		if (down & KEY_B) {
			debug = ~debug;
		}

		if (held & KEY_TOUCH) {
			static int last_touch_x;
			static int last_touch_y;
			if (last_touch_x != touch.px || last_touch_y != touch.py) {
				crosshair.moveTo(
						touch.px - crosshair.getScreenX(), 
						touch.py - crosshair.getScreenY(), 
						10
				);
				last_touch_x = touch.px;
				last_touch_y = touch.py;
			}
			crosshair.setShown(true);

			if (held & KEY_L) {
				// Shooting to the crosshair
				Projectile* projectile = new Projectile(ship, crosshair);
				sprites.push_back(projectile);
			}

		} else {
			crosshair.setShown(false);
		}
		
		
		int start_draw_sprite = current_ms;
		int removed_sprites = draw_all_sprites();
		int stop_draw_sprite = current_ms;

		// Update monsters
		update_monsters();

		// flip screens
		flip_vram();

		// Clear console & Write debugging infos
		if (debug) {
			printf("frame: %d, current_ms:%d\n", current_frame, current_ms);
			printf("nb sprites: %d, removed: %d\n", sprites.size(), removed_sprites);
			printf("draw: %dms, erase: %d\n", 
					stop_draw_sprite - start_draw_sprite,
					stop_erase - start_erase
				);
			
			printf("ship:(%d,%d), ch:(%d,%d)\n", 
					int(ship.getScreenX()), int(ship.getScreenY()), 
					int(crosshair.getScreenX()), int(crosshair.getScreenY())
				);
		}
		
		// erase back screen
		start_erase = current_ms;
		erase_screen(back);
		stop_erase = current_ms;

		PA_CheckLid();
		int start_wait_ms = current_ms;
		swiWaitForVBlank();
		int stop_wait_ms = current_ms;

		int free_ms = stop_wait_ms - start_wait_ms;
		int usage_ms = start_wait_ms - start_frame_ms;
		int cpu_free_percent = 100.0 * usage_ms / (usage_ms + free_ms);
		draw_cpu_usage(cpu_free_percent);

		if (debug) printf("\x1b[2J");
	}
	
	return 0;
} // End of main()
