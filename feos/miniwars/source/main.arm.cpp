#include <stdio.h>
#include <string.h>
#include "miniwars.h"
#include "font.h"

// the frame buffer
static u8 buf[192][256];

// the main bg palette
static u16 pal[256] = {
  RGB15( 0,  0,  0), // this is the backdrop color (transparent otherwise)
  RGB15( 0, 31,  0), // player color
};

static inline int remap(int pos, int from_start, int from_end, int to_start, int to_end) {
  return (pos-from_start)*(to_end-to_start)/(from_end-from_start) + to_start;
}

int main() {
  int    down, held;       // button states
  int    frame   = 0;      // frame counter
  u32    upgrade = 1;      // weapon upgrade level
  touchPosition stylus;    // touch coordinates
  list<Particle*> *pList;  // particle list
  Console *console;        // text console
  Player player;           // the player
  list<Enemy*> enemies;    // the enemies

  // initialize the player
  player.setPosition(0, 0);
  player.setSpeed(floattof32(0.3f));
  player.pickup(new Plasma());
  player.pickup(new Mortar());
  player.pickup(new Shotgun());

  // create a particle list
  pList = new list<Particle*>;

#ifdef FEOS
  // switch to FeOS direct mode
  FeOS_DirectMode();

  // disable FeOS auto-updates
  FeOS_SetAutoUpdate(AUTOUPD_OAM,  0);
  FeOS_SetAutoUpdate(AUTOUPD_BG,   0);
  FeOS_SetAutoUpdate(AUTOUPD_KEYS, 0);
#endif

  // put the main screen on the bottom
  lcdMainOnBottom();

  // set up the video modes
  videoSetMode(MODE_5_2D);
  videoSetModeSub(MODE_0_2D);

  // set up VRAM's primary banks
  vramSetPrimaryBanks(VRAM_A_MAIN_BG,
                      VRAM_B_MAIN_SPRITE,
                      VRAM_C_SUB_BG,
                      VRAM_D_SUB_SPRITE);

  // initialize the framebuffered background
  bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

  // initialize the main bg palette
  for(int i = 2; i < 9; i++) {
    int r = remap(i, 2, 8, 15, 31);
    int g = remap(i, 2, 8, 10, 31);
    int b = 0;
    pal[i] = RGB15(r, g, b);
    pal[17-i] = RGB15(r, g, b);
  }
  memcpy(BG_PALETTE, pal, sizeof(pal));

  // initialize the console background
  console = new Console(0, false, 2, 0);

  // initialize the console bg graphics and palette
  memcpy(bgGetGfxPtr(console->getBg()), fontTiles, fontTilesLen);
  memcpy(BG_PALETTE_SUB, fontPal, fontPalLen);

  // game loop
  do {
    swiWaitForVBlank();

    // copy framebuffer to VRAM
#ifdef ARM9
    DC_FlushRange(buf, sizeof(buf));
    dmaCopy(buf, bgGetGfxPtr(3), sizeof(buf));
#else
    memcpy(bgGetGfxPtr(3), buf, sizeof(buf));
#endif

    // print info to the console bg
    console->print(0, 0, "Weapon:    %-10s", player.getWeapon()->getName());
    console->print(0, 1, "Ammo:      %4u",   player.getWeapon()->getAmmo());
    console->print(0, 2, "Upgrade    %4u",   upgrade);
    console->print(0, 3, "Particles: %4d",   pList->size());
    console->print(0, 4, "Enemies:   %4d",   enemies.size());

    // clear the framebuffer
    memset(buf, 0, sizeof(buf));

    // add an enemy
    if(frame%900 == 0) {
      for(int i = 0; i < frame/1800+1; i++) {
        try {
          Enemy *e = new Enemy(&player, pList);
          e->setSpeed(floattof32(0.1f));
          try {
            switch(rand()%3) {
              case 0: e->pickup(new Plasma());  break;
              case 1: e->pickup(new Shotgun()); break;
              case 2: e->pickup(new Mortar());  break;
            }
          } catch(...) { delete e; throw; }

          enemies.push_back(e);
        } catch(...) {}
      }
    }

    // update the enemies
    for(list<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); it++) {
      Enemy *enemy = *it;
      enemy->update();
    }

    // update the button states
    scanKeys();
    down = keysDown();
    held = keysHeld();

    // update the upgrade level
    if(down & KEY_X) upgrade++;
    if(down & KEY_B) upgrade--;
    if(upgrade < 1)  upgrade = 1;
    if(upgrade > 4)  upgrade = 4;
    player.getWeapon()->setUpgrade(upgrade);

    // update the weapon type
    if(down & KEY_Y)
      player.nextWeapon();

    // move the player
    player.move((held|down));

    if((down|held) & KEY_TOUCH) {
      // get the latest touch coordinates
      touchRead(&stylus);

      // shoot the weapon
      player.getWeapon()->shoot(player.getX(), player.getY(), inttof32(stylus.px), inttof32(stylus.py), pList);
    }

    // update and draw the particles
    for(list<Particle*>::iterator it = pList->begin(); it != pList->end(); it++) {
      Particle *p = *it;
      p->move();
      if(p->isValid())
        buf[p->getY()][p->getX()] = p->getColor();
    }

    // draw enemies
    for(list<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); it++) {
      Enemy *e = *it;
      buf[f32toint(e->getY())][f32toint(e->getX())] = 1;
    }

    // draw player
    buf[f32toint(player.getY())][f32toint(player.getX())] = 1;

    // remove and clean up all of the stale particles
    for(list<Particle*>::iterator it = pList->begin(); it != pList->end(); it++) {
      Particle *p = *it;
      if(!p->isValid()) {
        it = pList->erase(it);
        delete p;
        if(it == pList->end())
          break;
      }
    }

    // increment the frame counter
    frame++;

    // update the player
    player.update();
  } while(!(down & KEY_START));

  // remove and clean up all of the particles
  for(list<Particle*>::iterator it = pList->begin(); it != pList->end(); it++) {
    Particle *p = *it;
    it = pList->erase(it);
    delete p;
    if(it == pList->end())
      break;
  }

  // remove and clean up all of the enemies
  for(list<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); it++) {
    Enemy *e = *it;
    delete e;
  }

  // clean up the particle list
  delete pList;

  // clean up the console
  delete console;

#ifdef FEOS
  // switch back to FeOS console mode
  FeOS_ConsoleMode();
#endif
 
  // exit the program
  return 0;
}

