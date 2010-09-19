// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __CROSSHAIR_H__
#define __CROSSHAIR_H__

#include "MovableSprite.h"

class Crosshair : public MovableSprite {
public:
	Crosshair();

	virtual int getSizeX() const;
	virtual int getSizeY() const;
	
private:
	int getSize() const;
};

#endif // __CROSSHAIR_H__
