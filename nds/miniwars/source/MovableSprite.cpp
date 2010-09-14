// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "MovableSprite.h"

#include <nds.h>
#include <cmath>

#include "nds_utils.h"

namespace {
	volatile int global_idx_sprite = 0;

	float sqr(float a) {
		return a * a;
	}

	int getSpriteSizeX(SpriteSize size) {
		switch (size) {
			case SpriteSize_8x8:
				return 8;
			case SpriteSize_64x64:
				return 64;
			default:
				return 0;
		}
	}

	int getSpriteSizeY(SpriteSize size) {
		switch (size) {
			case SpriteSize_8x8:
				return 8;
			case SpriteSize_64x64:
				return 64;
			default:
				return 0;
		}
	}

	unsigned short int COLOR_BLUE = RGB15(0,0,31);
	void Put8bitPixel(int scr_x, int scr_y, 
			unsigned short int color) {
		// Don't put pixel outside screen
		if (scr_x < 0 || scr_x > SCREEN_WIDTH - 1) return;
		if (scr_y < 0 || scr_y > SCREEN_HEIGHT - 1) return;

		// Framebuffering put them
		back[scr_x + scr_y * SCREEN_WIDTH] = color;
	}

};

MovableSprite::MovableSprite(SpriteSize size) 
	: is_shown(false), color(RGB15(15, 15, 15))
{
	// Memory Allocation 
	this->idx_sprite = ::global_idx_sprite++;
	this->sizeX = getSpriteSizeX(size);
	this->sizeY = getSpriteSizeY(size);
}

void MovableSprite::setDestination(float x, float y, int current_frame, int dest_frame) {
	// Sets the starting pos to the current pos
	if (dest_frame > current_frame) {
		this->x = this->getScreenX(current_frame);
		this->y = this->getScreenY(current_frame);
	} else {
		// already there
		this->x = x;
		this->y = x;
	}

	this->dest_x = x;
	this->dest_y = y;
	this->frame = current_frame;
	this->dest_frame = dest_frame;

}

float MovableSprite::getScreenX(int current_frame) const {
	int nb_frames_to_target = dest_frame - frame;
	if (getFramesLeft(current_frame) <= 0) {
		return dest_x;
	}

	return dest_x - (dest_x - x) * getFramesLeft(current_frame) / nb_frames_to_target;
}

float MovableSprite::getScreenY(int current_frame) const {
	int nb_frames_to_target = dest_frame - frame;
	if (getFramesLeft(current_frame) <= 0) {
		return dest_y;
	}

	return dest_y - (dest_y - y) * getFramesLeft(current_frame) / nb_frames_to_target;
}
	
int MovableSprite::getSizeX(int current_frame) const {
	return sizeX;
}

int MovableSprite::getSizeY(int current_frame) const {
	return sizeY;
}

int MovableSprite::getFramesLeft(int current_frame) const {
	return dest_frame - current_frame;
}


bool MovableSprite::isExpired(int current_frame) const {
	// By default, sprites never expire
	return false;
}

bool MovableSprite::setShown(bool is_shown) {
	bool old_value = this->is_shown;
	this->is_shown = is_shown;
	return old_value;
}

bool MovableSprite::draw(int current_frame) const {
	if (! this->is_shown) return false;
	
	int src_x = getScreenX(current_frame);
	int src_y = getScreenY(current_frame);
	int current_size_x = getSizeX(current_frame);
	int current_size_y = getSizeY(current_frame);
	for (int ix = - current_size_x/2; ix < current_size_x/2; ix++) {
		for (int iy = - current_size_y/2; iy < current_size_y/2; iy++) {
			Put8bitPixel(src_x + ix, src_y + iy, this->color);
		}
	}
	return true;
}

void MovableSprite::moveTo(float dx, float dy, int current_frame, float speed) {
	float distance = sqrt(sqr(dx) + sqr(dy));
	int nb_frame_needed = distance / speed;

	this->x = getScreenX(current_frame);
	this->y = getScreenY(current_frame);
	this->setDestination(
			this->x + dx, this->y + dy, 
			current_frame, current_frame + nb_frame_needed
		);
}
