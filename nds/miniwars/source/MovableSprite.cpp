// Copyright 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "MovableSprite.h"

#include <nds.h>

namespace {
	bool is_init_done = false;
	int idx_sprite = 0;
};

MovableSprite::MovableSprite(SpriteSize size) 
	: is_shown(false), size(size)
{
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
			size,
			SpriteColorFormat_256Color
		);

	this->idx_sprite = ::idx_sprite++;
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

