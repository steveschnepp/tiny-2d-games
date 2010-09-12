// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Crosshair.h"

Crosshair::Crosshair() 
	: MovableSprite(SpriteSize_8x8)
{
}

int Crosshair::getSize(int current_frame) const {
	const int offset = 15;
	if (current_frame > dest_frame + offset) return 8;

	return (dest_frame + offset - current_frame) + 8;
}

int Crosshair::getSizeX(int current_frame) const {
	return getSize(current_frame);
}
int Crosshair::getSizeY(int current_frame) const {
	return getSize(current_frame);
}
