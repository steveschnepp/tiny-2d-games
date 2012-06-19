#pragma once
#ifndef WEAPON_H
#define WEAPON_H

#ifdef FEOS
#include <feos.h>
#else
#include <nds.h>
#endif

class Weapon {
protected:
  u32 ammo;
  bool reloading;
public:
  Weapon(u32 ammo) : ammo(ammo), reloading(false) {}

  virtual void shoot(s32 x, s32 y, s32 tx, s32 ty, u32 upgrade, list<Particle*> *plist) = 0;
  virtual void update(u32 upgrade) = 0;
  virtual const char* getName() const = 0;
  u32 const getAmmo() const { return ammo; }
};

#endif /* WEAPON_H */

