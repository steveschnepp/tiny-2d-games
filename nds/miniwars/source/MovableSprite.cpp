// Copyright 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "MovableSprite.h"

#include <nds.h>

MovableSprite::MovableSprite() 
	: is_shown(false)
{
	init_sprite();
}

bool is_init_done = false;
int idx_sprite = 0;

void MovableSprite::init_sprite() {
	static int sprite_x = 8;
	static int sprite_y = 8;

	if (! is_init_done) {
		oamInit(&oamMain, SpriteMapping_1D_32, false);
		// Create the main palette
		for (int color = 0; color < 256; color++) {
			SPRITE_PALETTE[color] = (color << 7) | (1 << 15);
		}
			
		SPRITE_PALETTE[1] = RGB15(31, 0, 0);
		SPRITE_PALETTE[2] = RGB15(0, 31, 0);
		SPRITE_PALETTE[3] = RGB15(0, 0, 31);
		SPRITE_PALETTE[4] = RGB15(0, 0,  5);
	}

	// Memory Allocation 
	this->gfx = oamAllocateGfx(
			&oamMain, 
			SpriteSize_8x8,
			SpriteColorFormat_256Color
		);

	this->idx_sprite = ::idx_sprite++;

	// Fill the sprite with indexes colors
	for (int j = 0; j < sprite_y; j++) {
		for (int i = 0; i < sprite_x/2; i++) {
			int offset = j * sprite_x/2 + i;
                	this->gfx[offset] = this->idx_sprite * 7;
		}
        }
}

void MovableSprite::setDestination(float x, float y, int current_frame, int dest_frame) {
	this->dest_x = x;
	this->dest_y = y;
	this->frame = current_frame;
	this->dest_frame = dest_frame;
}

int MovableSprite::getScreenX(int current_frame) {
	int nb_frames_to_target = dest_frame - frame;

	return dest_x - (dest_x - x) * getFramesLeft(current_frame) / nb_frames_to_target;
}

int MovableSprite::getScreenY(int current_frame) {
	int nb_frames_to_target = dest_frame - frame;

	return dest_y - (dest_y - y) * getFramesLeft(current_frame) / nb_frames_to_target;
}

int MovableSprite::getFramesLeft(int current_frame) {
	return dest_frame - current_frame;
}
