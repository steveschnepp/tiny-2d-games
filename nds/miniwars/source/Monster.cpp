// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Monster.h"

#include "game_world.h"
#include "nds_utils.h"
#include "colors.h"

Monster::Monster(int x, int y, const MovableSprite& target) 
	: MovableSprite(SpriteSize_64x64), target(target), life(16)
{
	this->is_shown = true;
	this->x = x;
	this->y = y;
	this->color = RGB15_red;
}

void Monster::update() {
	// We are where we are. 
	// Forget about the past
	this->setPosition(this->getScreenX(), this->getScreenY());
	this->setDestination(target.getScreenX(), target.getScreenY(), float(0.1));
}

bool Monster::isDead() const {
	return (life <= 0);
}

bool Monster::draw() const {

	int x = this->getScreenX();
	int y = this->getScreenY();

	int szx = this->life;
	int szy = this->life;

	Put8bitLine(
		x, y - szy / 2,
		x + szx / 2, y,
		color
	);
	Put8bitLine(
		x + szx / 2, y,
		x, y + szy / 2,
		color
	);
	Put8bitLine(
		x, y + szy / 2,
		x - szx / 2, y,
		color
	);
	Put8bitLine(
		x - szx / 2, y,
		x, y - szy / 2,
		color
	);

	return true;
}
