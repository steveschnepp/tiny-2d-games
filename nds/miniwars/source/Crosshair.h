// Copyright 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __CROSSHAIR_H__
#define __CROSSHAIR_H__

#include "MovableSprite.h"

class Crosshair : public MovableSprite {
public:
	Crosshair();

private:
	void fillSprite();
};

#endif // __CROSSHAIR_H__
