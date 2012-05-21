// Thx to https://github.com/mtheall for the skel :)
#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>

#include "fixed.h"

u8  myBmp[192][256];

u32 xor128(void) {
  static u32 x = 123456789;
  static u32 y = 362436069;
  static u32 z = 521288629;
  static u32 w = 88675123;
  u32 t;
 
  t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

struct Entity {
	Entity() : x(0), y(0), dx(0), dy(0), color(0) {}
	f32 x, y;
	f32 dx, dy;
	u8 color;
};

const size_t NUM_ENTITIES = 4096 * 10;
std::list<Entity*> myEntities;

template <class T> const T& normalize(const T& value, const T& min, const T& max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

void drawPix(f32 fx, f32 fy, u8 color) {
	int x = fx.getInt();
	int y = fy.getInt();
	if (x >= 0 && x < 256 && y >= 0 && y < 192) myBmp[y][x] = color;
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
	BG_PALETTE[0] = RGB15(0, 0, 0);
	BG_PALETTE[1] = RGB15(31, 0, 0);
	BG_PALETTE[2] = RGB15(0, 31, 0);
	BG_PALETTE[3] = RGB15(0, 0, 31);

	// 16 colors for Entities
	for(u32 i = 16; i < 32; i++) { 
		BG_PALETTE[i] = RGB15(i, i, i);
	}

	Entity dst;
	dst.color = 1;
	Entity src;
	src.color = 2;

	// do stuff
	do {
		frame++;
		cpuStartTiming(0);
		// clear the buffer
		memset(myBmp, 0, sizeof(myBmp));

		if (down & KEY_R) {
			// Add 16 particles
			for (size_t i = 0; i < 16; i ++) {
				Entity* e = new Entity();
				e->x     = src.x;
				e->y     = src.y;

				e->dx     = (xor128()%256 - 128) / 256.0f;
				e->dy     = (xor128()%256 - 128) / 256.0f;

				e->color = xor128()%16+16; // don't allow transparent
				myEntities.push_back(e);
			}
		}

		if (down & KEY_L) {
			// Remove 16 particles @ front
			for (size_t i = 0; i < 16 && myEntities.size() > 0; i ++) {
				auto first = myEntities.begin();
				delete *first;
				myEntities.erase(first);
			}
		}

      if (down & KEY_TOUCH) {

	touchPosition touch;
	touchRead(&touch);
      
	dst.x = inttof32(touch.px);
	dst.y = inttof32(touch.py);

      }

	if (down & KEY_DOWN) {
              src.y += 1.0f;
        }
	if (down & KEY_UP) {
              src.y -= 1.0f;
        }
	if (down & KEY_LEFT) {
              src.x -= 1.0f;
        }
	if (down & KEY_RIGHT) {
              src.x += 1.0f;
        }

	// move the entities
	for(auto it = myEntities.begin(); it != myEntities.end(); it++) {
      Entity* e = *it;


        f32 dist_2 = sqr(dst.x - e->x) + sqr(dst.y - e->y);
	if ( dist_2 < 6.0f) {
		// We just reached dst
		it = myEntities.erase(it);
		delete(e);
		continue;
	} else {
		// Every particle wants to go @ dst
		f32 ax = (dst.x - e->x) / dist_2;
		f32 ay = (dst.y - e->y) / dist_2;

		e->dx += ax;
		e->dy += ay;
	}

      // move the entity
      e->x += e->dx;
      e->y += e->dy;

      // Loosing 0.1% velocity each frame
      e->dx *= 0.99f; 
      e->dy *= 0.99f;

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
	drawPix(e->x, e->y, e->color);
    }

    for (int i = -2; i <= 2; i++) {
    	for (int j = -2; j <= 2; j++) {
		drawPix(dst.x + inttof32(i), dst.y + inttof32(j), dst.color);
		drawPix(src.x + inttof32(i), src.y + inttof32(j), src.color);
	}
    }

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
	printf("nb: %d\n", myEntities.size());
	printf("cpu: %.0f%%\n", cpu_usage / 1.92f);
	printf("ticks: %d/%d (%.0f)\n", ticks_move, ticks_done, (100.0f * ticks_move / ticks_done));
    }

    // copy to vram
    memcpy(bgGetGfxPtr(main), myBmp, sizeof(myBmp));
  } while(true);

  return 0;
}
