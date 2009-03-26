// Drawing on DS is easy... This'll show you to what point !
// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>

// Includes
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>

#include <ext/slist>

#define DMA_COPY

const int MAX_X = SCREEN_WIDTH;
const int MAX_Y = SCREEN_HEIGHT;

int gravity = 1;
int escape_velocity = 640;
int nb_particles_per_frame = 128;

static uint16* front = VRAM_A;
static uint16* back = VRAM_B;
void flip_vram()
{
	if (front == VRAM_A) {
		front = VRAM_B;
		back = VRAM_A;
		videoSetMode(MODE_FB1);			
	} else {
		front = VRAM_A;
		back = VRAM_B;
		videoSetMode(MODE_FB0);			
	}
}

int PA_CheckLid();
void checkReset(int keys);

static inline void dmaFillWordsAsynch(const void* src, void* dest, uint32 size) {
	DMA_SRC(3) = (uint32)src;
	DMA_DEST(3) = (uint32)dest;
	DMA_CR(3) = DMA_SRC_FIX | DMA_COPY_WORDS | (size>>2);
	while(DMA_CR(3) & DMA_BUSY);
}


struct Particle {
	int x;
	int y;
	int dx;
	int dy;
	bool is_offscreen;
	const unsigned short int color;

	Particle(int initial_x, int initial_y, unsigned short int initial_color) 
		: is_offscreen(false), color(initial_color)
	{
		setX(initial_x);
		setY(initial_y);
		
		int initial_velocity = (rand() % escape_velocity) - escape_velocity / 2; 
		short initial_angle = (rand() % 0xFFFF); 

		dx = (initial_velocity * sinLerp(initial_angle)) >> 12;
		dy = (initial_velocity * cosLerp(initial_angle)) >> 12;
	}

	inline int getX() {
		return x >> 8;
	}
	inline int getY() {
		return y >> 8;
	}
	inline void setX(int new_x) {
	x = new_x << 8;
	}
	inline void setY(int new_y) {
		y = new_y << 8;
	}

	inline void move() {
		if (is_offscreen) return;
		// Gravity
		dy += gravity;

		x += dx;
		y += dy;

		if (getX()<0) {
			setX(0);
			dx = -dx;
		}
		if (getX() > MAX_X - 1) {
			setX(MAX_X - 1);
			dx = -dx;
		}
		if (getY()<0) {
			setY(0);
			dy = -dy;
		}
		if (getY() > MAX_Y - 1) {
			y = MAX_Y - 1;

			is_offscreen = true;
		}
	}

	inline void Put8bitPixel(int x, int y, unsigned short int color) {
		back[x + y * SCREEN_WIDTH] = color;
	}

	inline void hide() {
		Put8bitPixel(getX(), getY(), RGB15(0,0,0));
	}
	inline void show() {
		if (is_offscreen) return;
		Put8bitPixel(getX(), getY(), color);
	}
};

typedef __gnu_cxx::slist<Particle*> particles_list;

particles_list particles;

void initVideo() {
    /*
     *  Map VRAM to display a background on the main and sub screens.
     * 
     *  The vramSetMainBanks function takes four arguments, one for each of the
     *  major VRAM banks. We can use it as shorthand for assigning values to
     *  each of the VRAM bank's control registers.
     *
     *  We map banks A and B to main screen background memory. This gives us
     *  256KB, which is a healthy amount for 16-bit graphics.
     *
     *  We map bank C to sub screen background memory.
     *
     *  We map bank D to LCD. This setting is generally used for when we aren't
     *  using a particular bank.
     */
    /*vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000,
                     VRAM_B_MAIN_BG_0x06020000,
                     VRAM_C_SUB_BG_0x06200000,
                     VRAM_D_LCD);
	*/
    vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_SUB_SPRITE);							

	//set main display to render directly from the frame buffer
	videoSetMode(MODE_FB0);
	
    // /*  Set the video mode on the main screen. */
    /* videoSetMode(MODE_5_2D | // Set the graphics mode to Mode 5
                 DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                 DISPLAY_BG3_ACTIVE); //Enable BG3 for display
*/
    /*  Set the video mode on the sub screen. */
    videoSetModeSub(MODE_5_2D | // Set the graphics mode to Mode 5
                    DISPLAY_BG3_ACTIVE); // Enable BG3 for display
}

void initBackgrounds() {
    /*  Set up affine background 3 on main as a 16-bit color background. */
    REG_BG3CNT = BG_BMP16_256x256 |
                 BG_BMP_BASE(0) | // The starting place in memory
                 BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    REG_BG3PA = 1 << 8;
    REG_BG3PB = 0;
    REG_BG3PC = 0;
    REG_BG3PD = 1 << 8;

    /*  Place main screen background 3 at the origin (upper left of the
     *  screen).
     */
    REG_BG3X = 0;
    REG_BG3Y = 0;

    /*  Set up affine background 2 on main as a 16-bit color background. */
    REG_BG2CNT = BG_BMP16_128x128 |
                 BG_BMP_BASE(8) | // The starting place in memory
                 BG_PRIORITY(2);  // A higher priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 1 << 8;

    /*  Place main screen background 2 in an interesting place. */
    REG_BG2X = -(SCREEN_WIDTH / 2 - 32) << 8;
    REG_BG2Y = -32 << 8;

    /*  Set up affine background 3 on the sub screen as a 16-bit color
     *  background.
     */
    REG_BG3CNT_SUB = BG_BMP16_256x256 |
                     BG_BMP_BASE(0) | // The starting place in memory
                     BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the sub screen background 3
     *  to be the identity matrix.
     */
    REG_BG3PA_SUB = 1 << 8;
    REG_BG3PB_SUB = 0;
    REG_BG3PC_SUB = 0;
    REG_BG3PD_SUB = 1 << 8;

    /*
     *  Place main screen background 3 at the origin (upper left of the screen)
     */
    REG_BG3X_SUB = 0;
    REG_BG3Y_SUB = 0;
}

// Function: main()
int main(int argc, char *argv[]) {
	/*  Turn on the 2D graphics core. */
    powerOn(POWER_ALL_2D);
 
	 /*  Configure the VRAM and background control registers. */
    lcdMainOnBottom(); // Place the main screen on the bottom physical screen
    initVideo(); 
    // initBackgrounds(); 
	
	consoleDemoInit();
	// BG_PALETTE_SUB[255] = RGB15(31,31,31);	//by default font will be rendered with color 255
	
	// Infinite loop to keep the program running
	while (1) {
		static volatile int frame = 0;
		frame++;
		
		static touchPosition touch;
		touchRead(&touch);
		
		scanKeys();
		int held = keysHeld();

		// checkReset(held);

		frame ++;
		if (held & KEY_UP) {
			gravity ++;
		} else if (held & KEY_DOWN) {
			gravity --;
			if (gravity < 0) gravity = 0;
		}
		
		if (held & KEY_RIGHT) {
			escape_velocity ++;
		} else if (held & KEY_LEFT) {
			escape_velocity --;
			if (escape_velocity < 0) escape_velocity = 0;
		}
		
		if (held & KEY_A) {
			nb_particles_per_frame ++;
		} else if (held & KEY_B) {
			nb_particles_per_frame --;
			if (nb_particles_per_frame < 1) nb_particles_per_frame = 1;
		}

		if (held & KEY_TOUCH) {
			int random_int = rand();
			uint8 r_comp = (random_int & 0x0F00) >> 8;
			uint8 g_comp = (random_int & 0x00F0) >> 4;
			uint8 b_comp = (random_int & 0x000F) >> 0;
			uint16 color = RGB15(r_comp, g_comp, b_comp) | (1 << 15);

			// Add nb_particles_per_frame particle
			for (int i = 0; i < nb_particles_per_frame; i++) {
				Particle* particle = new Particle(touch.px, touch.py, color);
				particles.push_front(particle);
			}
		}

		// Moves every Particle
		particles_list::iterator i_old;
		for(particles_list::iterator i = particles.begin(); i != particles.end(); ++i) {
			Particle* particle = *i;
			particle->move();
			if (particle->is_offscreen) {
				if (i == particles.begin()) {
					i = particles.erase(i);
				} else {
					i = particles.erase_after(i_old);
				}
				delete (particle);
			} 
			i_old = i;
		}
		
#ifdef DMA_COPY
		// Wait while DMA Channel 3 is BUSY
		if (dmaBusy(3)) {
			int count = 0;
			while(dmaBusy(3)) { ++ count; }
		}
#endif
		
		// Draws every Particle on the back screen
		for(particles_list::iterator i = particles.begin(); i != particles.end(); ++i) {
			Particle* particle = *i;
			particle->show();
		}


		// flip screens
		flip_vram();

		// erase back screen
		uint16 col = RGB15(0, 0, 0) | BIT(15);
#ifdef DMA_COPY
		dmaFillWordsAsynch(&col, back, sizeof(uint16) * SCREEN_WIDTH * SCREEN_HEIGHT);
#else
		uint32 colcol = col | col << 16;
		swiFastCopy(&colcol, back, 192*256*2/4 | COPY_MODE_FILL);
#endif
		
		PA_CheckLid();
		swiWaitForVBlank();
	}
	
	return 0;
} // End of main()

inline bool PA_LidClosed() {
	return keysHeld() & KEY_LID; 
}

inline int PA_CheckLid() {
	if (!PA_LidClosed()) {
		return 0;
	}

	// Shutdown everything :p
	powerOff(POWER_ALL_2D); 

	// Wait for the lid to be opened again...
	while(PA_LidClosed()) {
		swiWaitForVBlank();
		scanKeys();
	}

	// Return the power !
	powerOn(POWER_ALL_2D);
	return 1;
}

inline void checkReset(int keys) {
	if (keys | KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R) {
		swiSoftReset();
	}
}
