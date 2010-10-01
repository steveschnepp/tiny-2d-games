// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __MONSTER_H__
#define __MONSTER_H__ 

#include "MovableSprite.h"

class Monster : public MovableSprite {
public:
	Monster(int x, int y, const MovableSprite& target);

	virtual bool draw() const;
	
	void update();
	bool isDead() const;

private:
	const MovableSprite& target;
	int life;
};

#endif // __MONSTER_H__
