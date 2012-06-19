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
#include "weapon.h"

class Plasma : public Weapon {
private:
  u32 phase;
  class pPlasma : public Particle {
  private:
    s32 dx;
    s32 dy;
    u8  color;
  public:
    pPlasma(s32 x, s32 y, s32 tx, s32 ty, s32 phase)
      : Particle(x,y), color(2)
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
    u8  getColor() { return (color = (color+1)%14)+2; }
  };

public:
  Plasma() : Weapon(100), phase(0) {}

  void shoot(s32 x, s32 y, s32 tx, s32 ty, u32 upgrade, list<Particle*> *pList) {
    if(!reloading && ammo > 2*upgrade) {
      ammo -= 2*upgrade;
      for(u32 i = 0; i < upgrade; i++)
        pList->push_back(new pPlasma(x, y, tx, ty, phase + i*degreesToAngle(360/upgrade)));
    }
    else if(!reloading) {
      reloading = true;
    }
  }

  void update(u32 upgrade) {
    phase += 2148;
    if(ammo < upgrade*100)
      ammo++;
    else
      reloading = false;
  }

  const char* getName() const { return "Plasma"; }
};

#endif /* PLASMA_H */

