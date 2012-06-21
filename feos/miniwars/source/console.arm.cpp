#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "miniwars.h"

Console::Console(int bg, bool main, int mapBase, int tileBase) {
  pal = 0;
  if(main)
    this->bg = bgInit(bg, BgType_Text4bpp, BgSize_T_256x256, mapBase, tileBase);
  else
    this->bg = bgInitSub(bg, BgType_Text4bpp, BgSize_T_256x256, mapBase, tileBase);
}

void Console::print(int x, int y, const char *format, ...) {
#define MAX_BUFFER 32*32
  static char msg[MAX_BUFFER];
  va_list arg_ptr;

  u8  *buf = (u8*)msg;
  u16 *map = bgGetMapPtr(bg) + (x+y*32);
  u16 *end = bgGetMapPtr(bg) + 32*32;

  va_start(arg_ptr, format);
  vsnprintf(msg, MAX_BUFFER, format, arg_ptr);
  va_end(arg_ptr);

  for(; map < end && *buf; buf++) {
    switch(*buf) {
      case '\n':
        map = (u16*)(((int)(map+32))%32);
        break;
      case ' ' ... '~':
        *map++ = ((*buf)-' ') | (pal<<12);
        break;
      default:
        *map++ = 0;
    }
  }
}

