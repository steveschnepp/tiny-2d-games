// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "MovableSprite.h"

#include <nds.h>
#include <cmath>

#include "nds_utils.h"
#include "game_world.h"
#include "colors.h"

namespace {
	volatile int global_idx_sprite = 0;

	float sqr(float a) {
		return a * a;
	}

	float nb_frame_needed(float dx, float dy, float speed) {
		float distance = sqrt(sqr(dx) + sqr(dy));
		return (distance / speed);
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
};

MovableSprite::MovableSprite(SpriteSize size) 
	: is_shown(false), color(RGB15_white)
{
	// Memory Allocation 
	this->idx_sprite = ::global_idx_sprite++;
	this->sizeX = getSpriteSizeX(size);
	this->sizeY = getSpriteSizeY(size);
}

void MovableSprite::setPosition(float x, float y) {
	this->setDestination(x, y, current_frame);
}

void MovableSprite::setDestination(float x, float y, unsigned int dest_frame) {
	// Sets the starting pos to the current pos
	if (dest_frame > current_frame) {
		this->x = this->getScreenX();
		this->y = this->getScreenY();
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

void MovableSprite::setDestination(float x, float y, float speed) {
	float dx = x - this->getScreenX();
	float dy = y - this->getScreenY();
	unsigned int new_dest_frame = current_frame + nb_frame_needed(dx, dy, speed);
	this->setDestination(x, y, new_dest_frame);
}

float MovableSprite::getScreenX() const {
	int nb_frames_to_target = dest_frame - frame;
	if (getFramesLeft() <= 0) {
		return dest_x;
	}

	return dest_x - (dest_x - x) * getFramesLeft() / nb_frames_to_target;
}

float MovableSprite::getScreenY() const {
	int nb_frames_to_target = dest_frame - frame;
	if (getFramesLeft() <= 0) {
		return dest_y;
	}

	return dest_y - (dest_y - y) * getFramesLeft() / nb_frames_to_target;
}
	
int MovableSprite::getSizeX() const {
	return sizeX;
}

int MovableSprite::getSizeY() const {
	return sizeY;
}

int MovableSprite::getFramesLeft() const {
	return dest_frame - current_frame;
}


bool MovableSprite::isExpired() const {
	// By default, sprites never expire
	return false;
}

bool MovableSprite::setShown(bool is_shown) {
	bool old_value = this->is_shown;
	this->is_shown = is_shown;
	return old_value;
}

bool MovableSprite::isShown() const {
	return this->is_shown;
}

bool MovableSprite::draw() const {
	int src_x = getScreenX();
	int src_y = getScreenY();
	int current_size_x = getSizeX();
	int current_size_y = getSizeY();

	Put8bitRect(
		src_x - current_size_x/2,
		src_y - current_size_y/2,
		src_x + current_size_x/2,
		src_y + current_size_y/2,
		color
	);

	return true;
}

void MovableSprite::moveTo(float dx, float dy, float speed) {
	unsigned int new_dest_frame = current_frame + nb_frame_needed(dx, dy, speed);
	this->setDestination(this->getScreenX() + dx, this->getScreenY() + dy, new_dest_frame);
}
