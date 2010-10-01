// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Projectile.h"

#include <cmath>

Projectile::Projectile(const MovableSprite& emitter, const MovableSprite& target) 
	: Particle(emitter)
{
	this->color = RGB15(15, 15, 0);

	float d_randx = rand() % target.getSizeX() - target.getSizeX() / 2;
	float d_randy = rand() % target.getSizeY() - target.getSizeY() / 2;

        this->setDestination(
			target.getScreenX() + d_randx, target.getScreenY() + d_randy, 
			float(1.0)
	);
}
