#ifdef FEOS
#include <feos.h>
#else
#include <nds.h>
#endif
#include <stdio.h>
#include <string.h>
#include "particle.h"
#include "plasma.h"
#include "shotgun.h"
#include "mortar.h"
#include "font.h"
#include "list.h"

// the frame buffer
static u8 buf[192][256];

// the weapon types
enum Weapon {
  PLASMA,
  SHOTGUN,
  MORTAR,
  NUM_WEAPONS,
};

// the weapon names
static const char *weapons[NUM_WEAPONS] = {
  "Plasma",
  "Shotgun",
  "Mortar",
};

// the main bg palette
static const u16 pal[256] = {
  RGB15( 0,  0,  0), // this is the backdrop color (transparent otherwise)
  RGB15( 0, 31,  0), // player color
  RGB15(31, 31,  0), // particle color
};

// print to a "console" bg
void print(const char *msg, int x, int y, int bg, int pal) {
  int len = strlen(msg);
  u16 *map = bgGetMapPtr(bg);

  for(int i = 0; i < len; i++)
    map[y*32+x+i] = (msg[i]-32) | (pal<<12);
}

int main() {
  int    down, held;       // button states
  int    frame   = 0;      // frame counter
  int    upgrade = 1;      // weapon upgrade level
  Weapon weapon  = PLASMA; // weapon type
  touchPosition stylus;    // touch coordinates
  list<Particle*> *pList;  // particle list
  struct {
    s32 x;
    s32 y;
    s32 speed;
  } player;

  // initialize the player
  player.x = 0;
  player.y = 0;
  player.speed = floattof32(0.5f);

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
  // initialize the console background
  bgInitSub(0, BgType_Text4bpp, BgSize_T_256x256, 2, 0);

  // initialize the main bg palette
  memcpy(BG_PALETTE, pal, sizeof(pal));

  // initialize the console bg graphics and palette
  memcpy(bgGetGfxPtr(4), fontTiles, fontTilesLen);
  memcpy(BG_PALETTE_SUB, fontPal,   fontPalLen);

  // game loop
  do {
    swiWaitForVBlank();

    // copy framebuffer to VRAM
    memcpy(bgGetGfxPtr(3), buf, sizeof(buf));

    // print info to the console bg
    static char str[32];
    snprintf(str, 32, "Weapon:    %-10s", weapons[weapon]);
    print(str, 0, 0, 4, 0);
    snprintf(str, 32, "Upgrade    %d", upgrade);
    print(str, 0, 1, 4, 0);
    snprintf(str, 32, "Particles: %4d", pList->size());
    print(str, 0, 2, 4, 0);

    // clear the framebuffer
    memset(buf, 0, sizeof(buf));

    // update the button states
    scanKeys();
    down = keysDown();
    held = keysHeld();

    // update the upgrade level
    if(down & KEY_X) upgrade++;
    if(down & KEY_B) upgrade--;
    if(upgrade < 1)  upgrade = 1;
    if(upgrade > 4)  upgrade = 4;

    // update the weapon type
    if(down & KEY_Y)
      weapon = (Weapon)((weapon - 1 + NUM_WEAPONS)%NUM_WEAPONS);
    if(down & KEY_A)
      weapon = (Weapon)((weapon + 1)%NUM_WEAPONS);

    // move the player
    switch((held|down) & (KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT)) {
      // cardinal directions
      case KEY_UP:
        player.y -= player.speed;
        break;
      case KEY_DOWN:
        player.y += player.speed;
        break;
      case KEY_LEFT:
        player.x -= player.speed;
        break;
      case KEY_RIGHT:
        player.x += player.speed;
        break;

      // diagonal directions
      case (KEY_UP|KEY_LEFT):
        player.x -= mulf32(floattof32(0.707106781f), player.speed);
        player.y -= mulf32(floattof32(0.707106781f), player.speed);
        break;
      case (KEY_UP|KEY_RIGHT):
        player.x += mulf32(floattof32(0.707106781f), player.speed);
        player.y -= mulf32(floattof32(0.707106781f), player.speed);
        break;
      case (KEY_DOWN|KEY_LEFT):
        player.x -= mulf32(floattof32(0.707106781f), player.speed);
        player.y += mulf32(floattof32(0.707106781f), player.speed);
        break;
      case (KEY_DOWN|KEY_RIGHT):
        player.x += mulf32(floattof32(0.707106781f), player.speed);
        player.y += mulf32(floattof32(0.707106781f), player.speed);
        break;

      // crazy directions !
      default:
        break;
    }

    // clamp the player position
    if(player.x < 0)              player.x = 0;
    if(player.x >= inttof32(256)) player.x = inttof32(256)-1;
    if(player.y < 0)              player.y = 0;
    if(player.y >= inttof32(192)) player.y = inttof32(192)-1;

    if((down|held) & KEY_TOUCH) {
      // get the latest touch coordinates
      touchRead(&stylus);

      // shoot the appropriate weapon
      switch(weapon) {
        case PLASMA:
          Plasma::shoot(player.x, player.y, inttof32(stylus.px), inttof32(stylus.py), frame*2148, upgrade, pList);
          break;

        case SHOTGUN:
          Shotgun::shoot(player.x, player.y, inttof32(stylus.px), inttof32(stylus.py), upgrade, pList);
          break;

        case MORTAR:
          Mortar::shoot(player.x, player.y, inttof32(stylus.px), inttof32(stylus.py), upgrade, pList);
          break;

        default:
          break;
      }
    }

    // update and draw the particles
    for(list<Particle*>::iterator it = pList->begin(); it != pList->end(); it++) {
      Particle *p = *it;
      p->move();
      if(p->isValid())
        buf[p->getY()][p->getX()] = 2;
    }

    // draw player
    buf[f32toint(player.y)][f32toint(player.x)] = 1;

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
  } while(!(down & KEY_START));

  // remove and clean up all of the particles
  for(list<Particle*>::iterator it = pList->begin(); it != pList->end(); it++) {
    Particle *p = *it;
    it = pList->erase(it);
    delete p;
    if(it == pList->end())
      break;
  }

  // clean up the particle list
  delete pList;

#ifdef FEOS
  // switch back to FeOS console mode
  FeOS_ConsoleMode();
#endif
 
  // exit the program
  return 0;
}

