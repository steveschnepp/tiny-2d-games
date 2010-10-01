// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __SHIP_H__
#define __SHIP_H__

#include "MovableSprite.h"

class Ship : public MovableSprite {
public:
	Ship();

	virtual bool draw() const;

private:
};

#endif // __SHIP_H__
