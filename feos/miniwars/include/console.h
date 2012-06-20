#pragma once
#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "miniwars.h"

class Console {
private:
  int bg;
  int pal;

public:
  Console(int bg, bool main, int mapBase, int tileBase);
  ~Console() {}

  void setPalette(int pal) { this->pal = pal; }
  int  getBg()             { return bg;       }
  int  getPal()            { return pal;      }

  void print(int x, int y, const char *format, ...);
};

#endif /* __CONSOLE_H */

