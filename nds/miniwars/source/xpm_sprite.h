// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __XPM_SPRITE_H__
#define __XPM_SPRITE_H__

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif
	
int allocate_palette_for_xpm(char xpm_sprite[], u16* sprite_palette, int from, int to); 
int load_xpm_in_sprite(char xpm_sprite[], u16* sprite, int sprite_size_x, int sprite_size_y); 

#ifdef __cplusplus
}
#endif

#endif // __XPM_SPRITE_H__
