// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Particle.h"

Particle::Particle(const MovableSprite& emitter, int current_frame) 
	: MovableSprite(SpriteSize_8x8)
{
	this->is_shown = true;
	this->x = emitter.getScreenX(current_frame);
	this->y = emitter.getScreenY(current_frame);
	this->dest_frame = 0;

	this->color = RGB15(15, 15, 0);
}

bool Particle::isExpired(int current_frame) const {
 // 	return (getFramesLeft(current_frame) <= 0);
	return false;
}
