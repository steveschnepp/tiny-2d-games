// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Projectile.h"

Projectile::Projectile(const MovableSprite& emitter, const MovableSprite& target) 
	: Particle(emitter)
{
	this->color = RGB15(15, 15, 0);

	float d_x = target.getScreenX() - this->x;
	float d_y = target.getScreenY() - this->y;
        this->moveTo(d_x, d_y, 1);
}

