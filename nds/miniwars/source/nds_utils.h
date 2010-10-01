// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __NDS_UTILS_H__
#define __NDS_UTILS_H__

// The function & global variables are to be reimplemented when porting to
// another target.

#include <nds.h>

#ifdef __cplusplus
   extern "C" {
#endif

// I/O
bool PA_CheckLid();
bool checkReset();

// GFX
void initGfx();

void Put8bitPixel(int scr_x, int scr_y, unsigned short int color);
void Put8bitLine(int scr_x1, int scr_y, int src_x2, int src_y2, unsigned short int color);
void Put8bitRect(int scr_x1, int scr_y, int src_x2, int src_y2, unsigned short int color);

void flip_vram();
void erase_screen(uint16* screen);

// Global variables 
extern uint16* front;
extern uint16* back;

#ifdef __cplusplus
   }
#endif

#endif // __NDS_UTILS_H__
