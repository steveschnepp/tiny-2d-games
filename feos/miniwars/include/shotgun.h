#pragma once
#ifndef SHOTGUN_H
#define SHOTGUN_H

#ifdef FEOS
#include <feos.h>
#else
#include <nds.h>
#endif
#include <stdlib.h>
#include "particle.h"
#include "atan.h"

class Shotgun {
private:
  class pShotgun : public Particle {
  private:
    s32 dx;
    s32 dy;
    s32 range;
  public:
    pShotgun(s32 x, s32 y, s32 tx, s32 ty, s32 range)
      : Particle(x,y), range(range)
    {
      s32 direction = atan2Tonc(ty-y, tx-x) + degreesToAngle(rand()%(range*2) - range);
      dx = cosLerp(direction);
      dy = sinLerp(direction);

      if(x < 0 || x > inttof32(256) || y < 0 || y > inttof32(192))
        valid = false;
    }

    void move() {
      x += dx;
      y += dy;
      if(x < 0 || x > inttof32(256) || y < 0 || y > inttof32(192))
        valid = false;
      if(--range < 0)
        valid = false;
    }

    s32 getX()     { return f32toint(x); }
    s32 getY()     { return f32toint(y); }
    bool isValid() { return valid;       }
  };

  Shotgun();
  ~Shotgun();
public:
  static void shoot(s32 x, s32 y, s32 tx, s32 ty, int upgrade, list<Particle*> *pList) {
    static int cooldown = 0;
    if(cooldown == 0) {
      for(int i = 0; i < 3*upgrade; i++)
        pList->push_back(new pShotgun(x, y, tx, ty, 5*upgrade));
      cooldown = 10 + 10/upgrade;
    }
    else
      cooldown--;
  }
};

#endif /* SHOTGUN_H */

