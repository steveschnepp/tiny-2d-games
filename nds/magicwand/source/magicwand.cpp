// Drawing on DS is easy... This'll show you to what point !

// Includes
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>

const int MAX_X = 512;
const int MAX_Y = 192;

int gravity = 1;
int escape_velocity = 64;

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


struct Particle {
	int x;
	int y;
	int dx;
	int dy;
	bool is_offscreen;

	Particle(int initial_x, int initial_y) 
		: is_offscreen(false) 
	{
		setX(initial_x);
		setY(initial_y);

		dx = (rand() % escape_velocity) - escape_velocity/2;
		dy = (rand() % escape_velocity) - escape_velocity/2;
	}

	int getX() {
		return x >> 8;
	}
	int getY() {
		return y >> 8;
	}
	void setX(int new_x) {
		x = new_x << 8;
	}
	void setY(int new_y) {
		y = new_y << 8;
	}

	void move() {
		if (is_offscreen) return;
		// Gravity
		dy += gravity;

		x += dx;
		y += dy;

		if (getX()<0) {
			x = 0;
			dx = 0;
		}
		if (getX() > MAX_X) {
			x = MAX_X;
			dx = 0;
		}
		if (getY()<0) {
			y = 0;
			dy = 0;
		}
		if (getY() > MAX_Y) {
			y = MAX_Y;
			dy = 0;
			is_offscreen = true;
		}
	}

	void Put8bitPixel(int x, int y, unsigned short int color) {
		front[x + y * SCREEN_WIDTH] = color;
	}

	void hide() {
		Put8bitPixel(getX(), getY(), RGB15(0,0,0));
	}
	void show() {
		if (is_offscreen) return;
		Put8bitPixel(getX(), getY(), RGB15(31,31,31));
	}
};

std::list<Particle*> particles;

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

		if (held & KEY_TOUCH) {
			// Add a particle
			Particle* particle = new Particle(touch.px, touch.py);
			particles.push_back(particle);
		}

		std::list<Particle*> new_particles;

		// Moves every Particle
		int counter = 0;
		for(std::list<Particle*>::iterator i = particles.begin(); i != particles.end(); ++i) {
			Particle* particle = *i;
			counter++;

			// particle->hide();
			particle->move();

			if (particle->is_offscreen) {
			// print at using ansi escape sequence \x1b[line;columnH 
				iprintf("\x1b[0;10HDelete particle number : %d   ", counter);
//				consoleDebugInit(DebugDevice_CONSOLE);
//				fprintf(stderr, "Delete particle number : %d/%d\n", counter, particles.size());
				i = particles.erase(i);
				delete (particle);
			} else {
				particle->show();
			}
		}

		// particles = new_particles;

		int line_number = 0;	
		iprintf("\x1b[0;%dHNumber of particles : %d   ", line_number++, particles.size());
		iprintf("\x1b[0;%dHGravity : %d   ", line_number++, gravity);
		iprintf("\x1b[0;%dHEscape velocity : %d   ", line_number++, escape_velocity);
		
		iprintf("\x1b[0;0HFrame : %d   ", frame);
		
		// PA_CheckLid();

		swiWaitForVBlank();

		
		// Wait while DMA Channel 3 is BUSY;
		if (dmaBusy(3)) {
			fprintf(stderr, "Wait while DMA Channel 3 is BUSY\n");
			int count = 0;
			while(dmaBusy(3)) { count ++; }
			fprintf(stderr, "Waited %d while DMA Channel 3 is BUSY\n", count);
		}
		
		// flip screens
		flip_vram();

		// erase back screen in async using DMA
		dmaCopyAsynch(RGB15(0,0,0), back, SCREEN_HEIGHT * SCREEN_WIDTH );
	}
	
	return 0;
} // End of main()
