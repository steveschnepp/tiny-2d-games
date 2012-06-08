// Drawing on DS is easy... This'll show you to what point !
// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include "bitmap.h"

u32 gravity = 1;
u32 escape_velocity = 640;
u32 nb_particles_per_frame = 128;

static u8 fb[SCREEN_WIDTH*SCREEN_HEIGHT];

struct Particle {
	s32 x;
	s32 y;
	s32 dx;
	s32 dy;
	bool is_offscreen;
	const u8 color;

	Particle() : x(0), y(0), dx(0), dy(0), is_offscreen(false), color(0) {}

	Particle(u32 initial_x, u32 initial_y, u8 initial_color)
		: is_offscreen(false), color(initial_color)
	{
		s32 initial_velocity = (rand() % escape_velocity) - escape_velocity / 2;
		s16 initial_angle    = (rand() % 0xFFFF);
		setX(initial_x);
		setY(initial_y);

		dx = (initial_velocity * sinLerp(initial_angle)) >> 12;
		dy = (initial_velocity * cosLerp(initial_angle)) >> 12;
	}

	s32 getX()           { return x >> 8;  }
	s32 getY()           { return y >> 8;  }
	void setX(s32 new_x) { x = new_x << 8; }
	void setY(s32 new_y) { y = new_y << 8; }

	void move() {
		if (is_offscreen)
			return;
		dy += gravity;

		x += dx;
		y += dy;

		if (getX() < 0) {
			setX(0);
			dx = -dx;
		}
		if (getX() > SCREEN_WIDTH - 1) {
			setX(SCREEN_WIDTH - 1);
			dx = -dx;
		}
		if (getY() < 0) {
			setY(0);
			dy = -dy;
		}
		if (getY() > SCREEN_HEIGHT - 1) {
			setY(SCREEN_HEIGHT - 1);
			is_offscreen = true;
		}
	}

	void show() {
		if (!is_offscreen)
			fb[getX() + getY() * SCREEN_WIDTH] = color;
	}
};

static inline void initVideo() {
	vramSetPrimaryBanks(VRAM_A_MAIN_BG,
	                    VRAM_B_MAIN_SPRITE,
	                    VRAM_C_SUB_BG,
	                    VRAM_D_SUB_SPRITE);

	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_0_2D);
}

static inline void initBackgrounds() {
	bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	for(u32 i = 1; i < 256; i++) {
		u32 r = rand();
		BG_PALETTE[i] = RGB15((r&0xF00) >> 6, (r&0xF0) >> 2, (r&0xF) << 2);
	}

	consoleDemoInit();
}

#define LIMIT 10240
static u32 bitmap[LIMIT/32] ALIGN(32);
static Particle particles[LIMIT] ALIGN(32);

int main(int argc, char *argv[]) {
	u32 frame = 0;
	u32 held  = 0;
	touchPosition touch;

	lcdMainOnBottom();
	initVideo();
	initBackgrounds();

	memset(bitmap, 0, sizeof(bitmap));

	while (1) {
		swiWaitForVBlank();
		memcpy(bgGetGfxPtr(3), fb, SCREEN_WIDTH*SCREEN_HEIGHT);
		iprintf("\x1b[0;0H");
		iprintf("Frame:     %5d\n", frame);
		iprintf("Particles: %5d\n", numSet(bitmap, LIMIT));
		iprintf("Gravity:   %5d\n", gravity);
		iprintf("Esc. Vel.: %5d\n", escape_velocity);
		iprintf("PPF:       %5d\n", nb_particles_per_frame);
		frame++;

		memset(fb, 0, SCREEN_WIDTH*SCREEN_HEIGHT);

		scanKeys();
		held = keysHeld();

		if (held & KEY_UP)
			gravity++;
		else if (held & KEY_DOWN && gravity > 0)
			gravity--;

		if (held & KEY_RIGHT)
			escape_velocity++;
		else if (held & KEY_LEFT && escape_velocity > 0)
			escape_velocity--;

		if (held & KEY_A)
			nb_particles_per_frame++;
		else if (held & KEY_B && nb_particles_per_frame > 1)
			nb_particles_per_frame--;

		if (held & KEY_TOUCH) {
			touchRead(&touch);

			for (u32 i = 0; i < nb_particles_per_frame; i++) {
				u32 bit = findClearBit(bitmap, LIMIT);
				if(bit != LIMIT) {
					new (&particles[bit]) Particle(touch.px, touch.py, rand()%255+1);
					setBit(bitmap, bit);
				}
			}
		}

		for(u32 i = findSetBit(bitmap, LIMIT); i < LIMIT; i++) {
			if(getBit(bitmap, i)) {
				Particle *p = &particles[i];
				p->move();
				if (p->is_offscreen)
					clearBit(bitmap, i);
				else
					p->show();
			}
		}
	}

	return 0;
}

