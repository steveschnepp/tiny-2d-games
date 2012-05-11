// Thx to https://github.com/mtheall for the skel :)
#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fixed.h"

u16 myPal[256];
u8  myBmp[192][256];

struct Entity {
	Entity() : x(0), y(0), dx(0), dy(0) {}
	int color;
	f32 x, y;
	f32 dx, dy;
};

const size_t NUM_ENTITIES = 4096 * 10;
Entity myEntities[NUM_ENTITIES];

f32 abs(f32 value) {
	if (value < 0) return -value; 
	return value; 
}

f32 sqr(f32 value) {
	return value * value; 
}

template <class T> const T& normalize(const T& value, const T& min, const T& max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

int main(int argc, char *argv[]) {
	int down = 0;
	static u32 frame = 0;

	// We use the touch screen for graphical stuff
	lcdMainOnBottom();

	// set up video mode
	videoSetMode(MODE_5_2D);
	// set up VRAM banks
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);

	// Set up console
	videoSetModeSub(MODE_0_2D);
	consoleDemoInit();

	// initialize the backgrounds
	int main = bgInit   (2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

	// clear the backgrounds
	memset(bgGetGfxPtr(main), 0, 256*256);

	// fill in the palette
	myPal[0] = RGB15(0, 0, 0);
	myPal[1] = RGB15(31, 0, 0);
	myPal[2] = RGB15(0, 31, 0);
	myPal[3] = RGB15(0, 0, 31);

	// 16 colors for Entities
	for(u32 i = 16; i < 32; i++) { 
		myPal[i] = RGB15(i, i, i);
	}
	memcpy(BG_PALETTE, myPal, sizeof(myPal));

	// initialize the entities
	for(u32 i = 0; i < NUM_ENTITIES; i++) {
		myEntities[i].x     = rand()%256 * 1.0f;
		myEntities[i].y     = rand()%192 * 1.0f;

		myEntities[i].dx     = (rand()%256 - 128) / 256.0f;
		myEntities[i].dy     = (rand()%256 - 128) / 256.0f;

		myEntities[i].color = rand()%16+16; // don't allow transparent
	}

	Entity dst;

	// do stuff
	unsigned int nb_particles = 0;
	do {
		frame++;
		cpuStartTiming(0);
		// clear the buffer
		memset(myBmp, 0, sizeof(myBmp));

		if (down & KEY_R) {
			nb_particles = normalize<int>(nb_particles + 16, 0, NUM_ENTITIES);
		}

		if (down & KEY_L) {
			nb_particles = normalize<int>(nb_particles - 16, 0, NUM_ENTITIES);
		}

		// move the entities
		for(u32 i = 0; i < NUM_ENTITIES; i++) {
			if (i > nb_particles) break; 
      Entity *e = &myEntities[i];

      if (down & KEY_TOUCH) {
	dst.color = 1;

	touchPosition touch;
	touchRead(&touch);

        f32 touchx = inttof32(touch.px);
        f32 touchy = inttof32(touch.py);

        f32 dist_2 = sqr(touchx - e->x) + sqr(touchy - e->y);
	if ( dist_2 < 4.0f) {
		e->dx = 0;
		e->dy = 0;
	} else {
		// Every particle wants to go @ dst
		f32 ax = (touchx - e->x) / dist_2;
		f32 ay = (touchy - e->y) / dist_2;

		e->dx += ax;
		e->dy += ay;
	}
      } else dst.color = 0;

      // move the entity
      e->x += e->dx;
      e->y += e->dy;

      // Loosing 0.1% velocity each frame
      e->dx *= 0.999f; 
      e->dy *= 0.999f;

      // clamp
      if(e->x > 255.0f) {
        e->x = 255.0f;
        e->dx = -(e->dx) ;
      }
      if(e->x < 0) {
        e->x = 0;
        e->dx = -(e->dx) ;
      }
      if(e->y > 191.0f) {
        e->y = 191.0f;
        e->dy = -(e->dy) ;
      }
      if(e->y < 0) {
        e->y = 0;
        e->dy = -(e->dy) ;
      }

      // copy to buffer
      myBmp[e->y.getInt()][e->x.getInt()] = e->color;
    }

    if (dst.color) myBmp[dst.y.getInt()][dst.x.getInt()] = dst.color;

    int ticks_move = cpuGetTiming();

    // wait for vblank
    int cpu_usage = REG_VCOUNT;
    swiWaitForVBlank();

    int ticks_done = cpuEndTiming();
    scanKeys();
    down = keysCurrent();

    // Print debug info
    { 
	consoleClear(); 
	printf("nb: %d\n", nb_particles);
	printf("cpu: %.0f%%\n", cpu_usage / 1.92f);
	printf("ticks: %d/%d (%.0f)\n", ticks_move, ticks_done, (100.0f * ticks_move / ticks_done));
    }

    // copy to vram
    memcpy(bgGetGfxPtr(main), myBmp, sizeof(myBmp));
  } while(true);

  return 0;
}
