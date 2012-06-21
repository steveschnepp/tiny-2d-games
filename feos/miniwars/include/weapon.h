#pragma once
#ifndef WEAPON_H
#define WEAPON_H

#include "miniwars.h"

class Weapon {
protected:
  u32 upgrade;
  u32 ammo;
  bool reloading;
public:
  Weapon(u32 ammo) : upgrade(1), ammo(ammo), reloading(false) {}
  virtual ~Weapon() {}

  virtual void shoot(s32 x, s32 y, s32 tx, s32 ty, list<Particle*> *plist) = 0;
  virtual void update() = 0;
  virtual const char* getName() const = 0;
  u32 const getAmmo()    const      { return ammo;             }
  void      addAmmo(u32 ammo)       { this->ammo += ammo;      }
  u32       getUpgrade() const      { return upgrade;          }
  void      setUpgrade(u32 upgrade) { this->upgrade = upgrade; }
};

#endif /* WEAPON_H */

