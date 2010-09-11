// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Crosshair.h"

Crosshair::Crosshair() 
	: MovableSprite(SpriteSize_8x8)
{
}

int Crosshair::getSize(int current_frame) const {
	if (current_frame > dest_frame) return 8;

	return (dest_frame + 120 - current_frame) + 8;
}

int Crosshair::getSizeX(int current_frame) const {
	return getSize(current_frame);
}
int Crosshair::getSizeY(int current_frame) const {
	return getSize(current_frame);
}
