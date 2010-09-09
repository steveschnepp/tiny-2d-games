// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "MovableSprite.h"

class Projectile : public MovableSprite {
public:
	Projectile();

private:
	void fillSprite();
};

#endif // __PROJECTILE_H__
