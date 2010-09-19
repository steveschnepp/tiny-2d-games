// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Particle.h"

#include "game_world.h"

Particle::Particle(const MovableSprite& emitter) 
	: MovableSprite(SpriteSize_8x8)
{
	this->is_shown = true;
	this->x = emitter.getScreenX();
	this->y = emitter.getScreenY();
	this->dest_frame = 0;

	this->color = RGB15(15, 15, 0);
}

bool Particle::isExpired() const {
 	return (getFramesLeft() <= 0);
}
