// Thx to mtheall for the skel :)
#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

u16 myPal[256];
u8  myBmp[192][256];

struct Entity {
  int color;
  float x, y;
  float dx, dy;
  inline int px() const { return int(x); }
  inline int py() const { return int(y); }
};

const size_t NUM_ENTITIES = 4096;
Entity myEntities[NUM_ENTITIES];

int main(int argc, char *argv[]) {
  int down;

  // We use the touch screen for graphical stuff
  lcdMainOnBottom();

  // set up video mode
  videoSetMode(MODE_5_2D);
  // set up VRAM banks
  vramSetBankA(VRAM_A_MAIN_BG);
  vramSetBankB(VRAM_B_MAIN_SPRITE);

  // Set up console
  videoSetModeSub(MODE_0_2D);
  consoleDemoInit();

  // initialize the backgrounds
  int main = bgInit   (2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

  // clear the backgrounds
  memset(bgGetGfxPtr(main), 0, 256*256);

  // fill in the palette
  myPal[0] = RGB15(0, 0, 0);
  for(u32 i = 1; i < sizeof(myPal); i++)
    myPal[i] = RGB15(rand()%31, rand()%31, rand()%31);
  memcpy(BG_PALETTE, myPal, sizeof(myPal));

  // initialize the entities
  for(u32 i = 0; i < NUM_ENTITIES; i++) {
    myEntities[i].x     = rand()%256;
    myEntities[i].y     = rand()%192;

    myEntities[i].dx     = (rand()%256 - 128) / 256.0;
    myEntities[i].dy     = (rand()%256 - 128) / 256.0;

    myEntities[i].color = rand()%255+1; // don't allow transparent
  }

  // do stuff
  do {
    // clear the buffer
    memset(myBmp, 0, sizeof(myBmp));

    // move the entities
    for(u32 i = 0; i < NUM_ENTITIES; i++) {
      Entity *e = &myEntities[i];

      // move the entity
      e->x += e->dx;
      e->y += e->dy;

      // clamp
      if(e->x > 255) {
        e->x = 255;
        e->dx = -(e->dx) ;
      }
      if(e->x < 0) {
        e->x = 0;
        e->dx = -(e->dx) ;
      }
      if(e->y > 191) {
        e->y = 191;
        e->dy = -(e->dy) ;
      }
      if(e->y < 0) {
        e->y = 0;
        e->dy = -(e->dy) ;
      }

      // copy to buffer
      myBmp[e->py()][e->px()] = e->color;
    }

    // wait for vblank
    int cpu_usage = REG_VCOUNT;
    swiWaitForVBlank();
    scanKeys();
    down = keysDown();

    // Print debug info
    { 
	consoleClear(); 
	static int frame = 0;
	printf("frame: %d\n", frame ++);
	printf("cpu: %d\n", cpu_usage);
    }

    // copy to vram
    memcpy(bgGetGfxPtr(main), myBmp, sizeof(myBmp));
  } while(!down);

  return 0;
}
