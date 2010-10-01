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
#include "colors.h"

#include "MovableSprite.h"
#include "Ship.h"
#include "Crosshair.h"
#include "Particle.h"
#include "Projectile.h"
#include "Monster.h"

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
	const unsigned int color_busy = CLR_green;
	const unsigned int color_free = CLR_DarkGreen;
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

	// Initialize the GFX Hardware
	initGfx();

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
