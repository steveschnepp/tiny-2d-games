// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <vector>

#include "MovableSprite.h"

class Particle : public MovableSprite {
public:
	Particle(const MovableSprite& emitter);

	virtual bool isExpired() const;
};

#endif // __PARTICLE_H__
