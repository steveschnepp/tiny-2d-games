#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"
#include "player.h"
#include "weapon.h"
#include "list.h"
#include "particle.h"

class Enemy : public Character {
private:
  Player *target;
  list<Particle*> *pList;
public:
  Enemy(Player *target, list<Particle*> *pList) : target(target), pList(pList) {
    switch(rand()%4) {
      case 0:
        setPosition(0, inttof32(rand()%192));
        break;
      case 1:
        setPosition(inttof32(256)-1, inttof32(rand()%192));
        break;
      case 2:
        setPosition(inttof32(rand()%256), 0);
        break;
      case 3:
        setPosition(inttof32(rand()%256), inttof32(192)-1);
        break;
    }
  }

  void update() {
    Weapon *weapon;
    for(list<Weapon*>::iterator it = weapons.begin(); it != weapons.end(); it++) {
      weapon = *it;
      weapon->update();
    }
    move(target);
    
    weapon = *it;
    weapon->shoot(x, y, target->getX(), target->getY(), pList);
  }
};
#endif /* ENEMY_H */

