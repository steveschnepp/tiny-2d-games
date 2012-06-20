#ifdef FEOS
#include <feos.h>
#else
#include <nds.h>
#endif
#include <string.h>
#include "list.h"
#include "character.h"
#include "weapon.h"

Character::~Character() {
  for(it = weapons.begin(); it != weapons.end(); it++) {
    Weapon *weapon = *it;
    delete weapon;
  }
}

void Character::pickup(Weapon *w) {
  if(!w)
    return;

  for(list<Weapon*>::iterator iter = weapons.begin(); iter != weapons.end(); iter++) {
    Weapon *weapon = *iter;
    if(strcmp(w->getName(), weapon->getName()) == 0) {
      if(w->getUpgrade() > weapon->getUpgrade())
        weapon->setUpgrade(w->getUpgrade());
      weapon->addAmmo(w->getAmmo());
      return;
    }
  }

  weapons.push_back(w);
  if(weapons.size() == 1)
    nextWeapon();
}

Weapon* Character::nextWeapon() {
  it++;
  if(it == weapons.end())
    it = weapons.begin();

  return 0;
}

void Character::move(int keys) {
  switch(keys & (KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT)) {
    case KEY_UP:
      y -= speed;
      break;
    case KEY_DOWN:
      y += speed;
      break;
    case KEY_LEFT:
      x -= speed;
      break;
    case KEY_RIGHT:
      x += speed;
      break;

    case (KEY_UP|KEY_LEFT):
      x -= mulf32(floattof32(0.707106781f), speed);
      y -= mulf32(floattof32(0.707106781f), speed);
      break;
    case (KEY_UP|KEY_RIGHT):
      x += mulf32(floattof32(0.707106781f), speed);
      y -= mulf32(floattof32(0.707106781f), speed);
      break;
    case (KEY_DOWN|KEY_LEFT):
      x -= mulf32(floattof32(0.707106781f), speed);
      y += mulf32(floattof32(0.707106781f), speed);
      break;
    case (KEY_DOWN|KEY_RIGHT):
      x += mulf32(floattof32(0.707106781f), speed);
      y += mulf32(floattof32(0.707106781f), speed);
      break;
  }
}

