// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Ship.h"

Ship::Ship() 
	: MovableSprite(SpriteSize_64x64)
{
	this->color = RGB15(0, 0, 15);
}
