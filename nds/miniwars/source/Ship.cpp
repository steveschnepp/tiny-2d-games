// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Ship.h"

#include "nds_utils.h"

Ship::Ship() 
	: MovableSprite(SpriteSize_64x64)
{
	this->color = RGB15(0, 0, 15);
}

bool Ship::draw() const {
	int x = this->getScreenX();
	int y = this->getScreenY();

	int szx = this->getSizeX();
	int szy = this->getSizeY();

	Put8bitLine(
		x - szx / 2,	y + szy / 2,
		x, 		y - szy / 2,
		color
	);
	Put8bitLine(
		x, 		y - szy / 2,
		x + szx / 2,	y + szy / 2,
		color
	);
	Put8bitLine(
		x + szx / 2,	y + szy / 2,
		x - szx / 2,	y + szy / 2,
		color
	);

	return true;
}
