// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "Particle.h"

class Projectile : public Particle {
public:
	Projectile(const MovableSprite& emitter, const MovableSprite& target);
};

#endif // __PROJECTILE_H__
