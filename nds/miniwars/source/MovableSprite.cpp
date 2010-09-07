// Copyright 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "MovableSprite.h"

#include <nds.h>
#include <cmath>

#include "nds_utils.h"

namespace {
	volatile int global_idx_sprite = 0;

	float sqr(float a) {
		return a * a;
	}

	void Put8bitPixel(int scr_x, int scr_y, 
			unsigned short int color) {
		back[scr_x + scr_y * SCREEN_WIDTH] = color;
	}

};

MovableSprite::MovableSprite(SpriteSize size) 
	: is_shown(false), size(size)
{
	// Memory Allocation 
	this->idx_sprite = ::global_idx_sprite++;
}

void MovableSprite::setDestination(float x, float y, int current_frame, int dest_frame) {
	this->dest_x = x;
	this->dest_y = y;
	this->frame = current_frame;
	this->dest_frame = dest_frame;
}

int MovableSprite::getScreenX(int current_frame) const {
	int nb_frames_to_target = dest_frame - frame;
	if (nb_frames_to_target <= 0) {
		return dest_x;
	}

	return dest_x - (dest_x - x) * getFramesLeft(current_frame) / nb_frames_to_target;
}

int MovableSprite::getScreenY(int current_frame) const {
	int nb_frames_to_target = dest_frame - frame;
	if (nb_frames_to_target <= 0) {
		return dest_y;
	}

	return dest_y - (dest_y - y) * getFramesLeft(current_frame) / nb_frames_to_target;
}

int MovableSprite::getFramesLeft(int current_frame) const {
	return dest_frame - current_frame;
}

int MovableSprite::getSizeX() const {
	switch (size) {
		case SpriteSize_8x8:
			return 8;
		case SpriteSize_64x64:
			return 64;
		default:
			return 0;
	}
}


int MovableSprite::getSizeY() const {
	switch (size) {
		case SpriteSize_8x8:
			return 8;
		case SpriteSize_64x64:
			return 64;
		default:
			return 0;
	}
}

bool MovableSprite::setShown(bool is_shown) {
	bool old_value = this->is_shown;
	this->is_shown = is_shown;
	return old_value;
}

bool MovableSprite::draw(int current_frame) const {
	if (! this->is_shown) return false;
	
	Put8bitPixel(getScreenX(current_frame), getScreenY(current_frame), RGB15(0,0,31));
	return true;
}

void MovableSprite::moveTo(float dx, float dy, int current_frame, float speed) {
	float distance = sqrt(sqr(dx) + sqr(dy));
	int nb_frame_needed = int(distance / speed);

	this->setDestination(
			this->x + dx, this->y + dy, 
			current_frame, nb_frame_needed
		);
}
