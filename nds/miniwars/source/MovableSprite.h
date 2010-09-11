// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __MOVABLE_SPRITE_H__
#define __MOVABLE_SPRITE_H__

#include <nds.h>

class MovableSprite {

protected:
	int frame;
	float x, y;
	
	int dest_frame;
	float dest_x, dest_y;

	bool is_shown;
	u16* gfx;
	int idx_sprite;
	int sizeX;
	int sizeY;
	
	int getFramesLeft(int current_frame) const;

public:

	virtual int getSizeX(int current_frame) const;
	virtual int getSizeY(int current_frame) const;

	MovableSprite(SpriteSize size);

	void setDestination(float x, float y, int current_frame, int dest_frame);
	
	void moveTo(float dx, float dy, int current_frame, float speed);
	float getScreenX(int current_frame) const;
	float getScreenY(int current_frame) const;

	bool draw(int current_frame) const;

	bool setShown(bool is_shown);
};

#endif // __MOVABLE_SPRITE_H__
