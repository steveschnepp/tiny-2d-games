// Copyright 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Crosshair.h"

Crosshair::Crosshair() 
	: MovableSprite(SpriteSize_64x64)
{
	fillSprite();
}

void Crosshair::fillSprite() {
	const int sprite_x = 64;
	const int sprite_y = 64;
	// Fill the sprite with indexes colors
	for (int j = 0; j < sprite_y; j++) {
		for (int i = 0; i < sprite_x/2; i++) {
			int offset = j * sprite_x/2 + i;
                	this->gfx[offset] = 2; 
                	this->gfx[offset+1] = 2; 
		}
        }
}

