// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Particle.h"

#include "nds_utils.h"
#include "game_world.h"

Particle::Particle(const MovableSprite& emitter) 
	: MovableSprite(SpriteSize_8x8)
{
	this->is_shown = true;

	// We are where the emitter is
	this->setPosition(emitter.getScreenX(), emitter.getScreenY());

	this->color = RGB15(15, 15, 0);
}

bool Particle::isExpired() const {
 	return (getFramesLeft() <= 0);
}

bool Particle::draw() const {
	Put8bitPixel(this->getScreenX(), this->getScreenY(), color);
}
