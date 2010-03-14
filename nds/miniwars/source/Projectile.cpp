// Copyright 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "Projectile.h"

Projectile::Projectile() 
	: MovableSprite(SpriteSize_8x8)
{
	fillSprite();
}

void Projectile::fillSprite() {
	const int sprite_x = 8;
	const int sprite_y = 8;
	// Fill the sprite with indexes colors
	for (int j = 0; j < sprite_y; j++) {
		for (int i = 0; i < sprite_x/2; i++) {
			int offset = j * sprite_x/2 + i;
                	this->gfx[offset] = this->idx_sprite * 7;
		}
        }
}

