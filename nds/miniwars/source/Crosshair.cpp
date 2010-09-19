// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Crosshair.h"

#include "game_world.h"

Crosshair::Crosshair() 
	: MovableSprite(SpriteSize_8x8)
{
}

int Crosshair::getSize() const {
	const int offset = 15;
	if (current_frame > dest_frame + offset) return 8;

	return (dest_frame + offset - current_frame) + 8;
}

int Crosshair::getSizeX() const {
	return getSize();
}
int Crosshair::getSizeY() const {
	return getSize();
}
