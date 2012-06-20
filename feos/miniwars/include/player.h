#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "miniwars.h"

class Player : public Character {
private:
public:
  Player() {}

  void update() {
    for(list<Weapon*>::iterator it = weapons.begin(); it != weapons.end(); it++) {
      Weapon *weapon = *it;
      weapon->update();
    }
  }
};

#endif /* PLAYER_H */

