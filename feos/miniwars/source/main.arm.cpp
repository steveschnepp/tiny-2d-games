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
#include "console.h"
#include "font.h"
#include "list.h"
#include "weapon.h"

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

static Shotgun shotgun;
static Plasma  plasma;
static Mortar  mortar;
struct WeaponList {
  Weapon     *current;
  WeaponList *prev;
  WeaponList *next;
};
static WeaponList pWeapons[] = {
  { &plasma,  &pWeapons[2], &pWeapons[1] },
  { &shotgun, &pWeapons[0], &pWeapons[2] },
  { &mortar,  &pWeapons[1], &pWeapons[0] },
};

int main() {
  int    down, held;       // button states
  int    frame   = 0;      // frame counter
  u32    upgrade = 1;      // weapon upgrade level
  touchPosition stylus;    // touch coordinates
  list<Particle*> *pList;  // particle list
  Console *console;        // text console
  struct {
    s32 x;
    s32 y;
    s32 speed;
    WeaponList *weapons = pWeapons;
  } player;

  // initialize the player
  player.x = 0;
  player.y = 0;
  player.speed = floattof32(0.3f);

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
    memcpy(bgGetGfxPtr(3), buf, sizeof(buf));

    // print info to the console bg
    console->print(0, 0, "Weapon:    %-10s", player.weapons->current->getName());
    console->print(0, 1, "Weapon:    %4u",   player.weapons->current->getAmmo());
    console->print(0, 2, "Upgrade    %4u",   upgrade);
    console->print(0, 3, "Particles: %4d",   pList->size());

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
      player.weapons = player.weapons->next;
    if(down & KEY_A)
      player.weapons = player.weapons->prev;

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

      // shoot the weapon
      player.weapons->current->shoot(player.x, player.y, inttof32(stylus.px), inttof32(stylus.py), upgrade, pList);
    }

    // update and draw the particles
    for(list<Particle*>::iterator it = pList->begin(); it != pList->end(); it++) {
      Particle *p = *it;
      p->move();
      if(p->isValid())
        buf[p->getY()][p->getX()] = p->getColor();
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

    // update the weapons
    for(u32 i = 0; i < sizeof(pWeapons)/sizeof(pWeapons[0]); i++)
      pWeapons[i].current->update(upgrade);
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

  // clean up the console
  delete console;

#ifdef FEOS
  // switch back to FeOS console mode
  FeOS_ConsoleMode();
#endif
 
  // exit the program
  return 0;
}

