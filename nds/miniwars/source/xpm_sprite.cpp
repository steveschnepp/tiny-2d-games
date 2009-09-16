// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>
#include "xpm_sprite.h"

struct xpm_header {
	int x;
	int y;
	int nb_chars_per_pixel;
	int nb_colors;
};

int read_xpm_header(const char xpm_sprite[], struct xpm_header* header);

int allocate_palette_for_xpm(char xpm_sprite[], u16* sprite_palette, int from, int to) {
	return 0;
}


int load_xpm_in_sprite(char xpm_sprite[], u16* sprite, int sprite_size_x, int sprite_size_y) {
	return 0;
}


