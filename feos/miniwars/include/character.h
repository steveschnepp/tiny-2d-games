#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H

#ifdef FEOS
#include <feos.h>
#else
#include <nds.h>
#endif
#include "weapon.h"
#include "list.h"

class Character {
protected:
  s32 x;
  s32 y;
  s32 speed;
  u32 health;
  list<Weapon*> weapons;
  list<Weapon*>::iterator it;
public:
  Character() : x(0), y(0), speed(0), health(0), weapons(), it(weapons.begin()) {}
  virtual ~Character();

  s32  getX()     const      { return x;              }
  s32  getY()     const      { return y;              }
  void setPosition(s32 x, s32 y)
                             { this->x = x;
                               this->y = y;           }
  s32  getSpeed() const      { return speed;          }
  void setSpeed(s32 speed)   { this->speed = speed;   }
  u32  getHealth() const     { return health;         }
  void setHealth(u32 health) { this->health = health; }

  void pickup(Weapon *w);
  Weapon* getWeapon()     { return *it; }
  Weapon* nextWeapon();
  u32  numWeapons() const { return weapons.size(); }

  void move(int keys);
  void move(Character *target);
  virtual void update() = 0;
};

#endif /* CHARACTER_H */

