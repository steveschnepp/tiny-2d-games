#pragma once
#ifndef PLASMA_H
#define PLASMA_H

#ifdef FEOS
#include <feos.h>
#else
#include <nds.h>
#endif
#include "particle.h"
#include "atan.h"
#include "list.h"

class Plasma {
private:
  class pPlasma : public Particle {
  private:
    s32 dx;
    s32 dy;
  public:
    pPlasma(s32 x, s32 y, s32 tx, s32 ty, s32 phase)
      : Particle(x,y)
    {
      s32 direction = atan2Tonc(ty-y, tx-x) + (sinLerp(phase)>>4);
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
    }

    s32 getX()     { return f32toint(x); }
    s32 getY()     { return f32toint(y); }
    bool isValid() { return valid;       }
  };

  Plasma();
  ~Plasma();
public:
  static void shoot(s32 x, s32 y, s32 tx, s32 ty, s32 phase, int upgrade, list<Particle*> *pList) {
    for(int i = 0; i < upgrade; i++)
      pList->push_back(new pPlasma(x, y, tx, ty, phase + i*degreesToAngle(360/upgrade)));
  }
};

#endif /* PLASMA_H */

