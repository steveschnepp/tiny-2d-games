#pragma once
#ifndef SHOTGUN_H
#define SHOTGUN_H

#include <stdlib.h>
#include "miniwars.h"

class Shotgun : public Weapon {
private:
  u32 cooldown;
  bool reloading;
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
      dx = cosLerp(direction)/2;
      dy = sinLerp(direction)/2;

      range *= 3;

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
    u8  getColor() { return 8;           }
  };

public:
  Shotgun() : Weapon(6), cooldown(0) {}

  void shoot(s32 x, s32 y, s32 tx, s32 ty, list<Particle*> *pList) {
    if(cooldown == 0) {
      for(u32 i = 0; i < 3*upgrade; i++)
        pList->push_back(new pShotgun(x, y, tx, ty, 3*upgrade+8));
      if(ammo && --ammo)
        cooldown = 15 + 15/upgrade;
      else {
        cooldown = 30 + 30/upgrade;
        reloading = true;
      }
    }
  }

  void update() {
    if(cooldown > 0)
      cooldown--;
    if(reloading && cooldown == 0) {
      ammo = 6*upgrade;
      reloading = false;
    }
  }

  const char* getName() const { return "Shotgun"; }
};

#endif /* SHOTGUN_H */

