// Thx to mtheall for the skel :)
#include <nds.h>
#include <stdlib.h>
#include <string.h>

u16 myPal[256];
u8  myBmp[192][256];

struct Entity {
  int x;
  int y;
  int color;
};
Entity myEntities[4096];
#define NUM_ENTITIES (sizeof(myEntities)/sizeof(myEntities[0]))

int main(int argc, char *argv[]) {
  int down;

  // set up video mode
  videoSetMode(MODE_5_2D);
  videoSetModeSub(MODE_5_2D);

  // set up VRAM banks
  vramSetPrimaryBanks(VRAM_A_MAIN_BG,
                      VRAM_B_MAIN_SPRITE,
                      VRAM_C_SUB_BG,
                      VRAM_D_SUB_SPRITE);

  // initialize the backgrounds
  int main = bgInit   (2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
  int sub  = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

  // clear the backgrounds
  memset(bgGetGfxPtr(main), 0, 256*256);
  memset(bgGetGfxPtr(sub),  0, 256*256);

  // fill in the palette
  myPal[0] = RGB15(0, 0, 0);
  for(u32 i = 1; i < sizeof(myPal); i++)
    myPal[i] = RGB15(rand()%31, rand()%31, rand()%31);
  memcpy(BG_PALETTE, myPal, sizeof(myPal));
  memcpy(BG_PALETTE_SUB, myPal, sizeof(myPal));

  // initialize the entities
  for(u32 i = 0; i < NUM_ENTITIES; i++) {
    myEntities[i].x     = rand()%256;
    myEntities[i].y     = rand()%192;
    myEntities[i].color = rand()%255+1; // don't allow transparent
  }

  // do stuff
  do {
    // clear the buffer
    memset(myBmp, 0, sizeof(myBmp));

    // move the entities
    for(u32 i = 0; i < NUM_ENTITIES; i++) {
      Entity *e = &myEntities[i];

      // move up to one pixel in each direction
      e->x += rand()%3-1;
      e->y += rand()%3-1;

      // clamp
      if(e->x > 255)
        e->x = 255;
      if(e->x < 0)
        e->x = 0;
      if(e->y > 191)
        e->y = 191;
      if(e->x < 0)
        e->x = 0;

      // copy to buffer
      myBmp[e->y][e->x] = e->color;
    }

    // wait for vblank
    swiWaitForVBlank();
    scanKeys();
    down = keysDown();

    // copy to vram
    memcpy(bgGetGfxPtr(main), myBmp, sizeof(myBmp));
    memcpy(bgGetGfxPtr(sub),  myBmp, sizeof(myBmp));
  } while(!down);

  return 0;
}
