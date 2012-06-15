#pragma once
#ifndef CONSOLE_H
#define CONSOLE_H

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

#endif /* CONSOLE_H */

