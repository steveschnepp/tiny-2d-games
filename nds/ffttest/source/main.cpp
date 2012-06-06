#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>

#include "fixed.h"
#include "fix_fft.h"

char  myBmp[192][256];

template <class T> const T& normalize(const T& value, const T& min, const T& max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

void drawPix(f32 fx, f32 fy, u8 color) {
	int x = fx.getInt();
	int y = fy.getInt();
	if (x >= 0 && x < 256 && y >= 0 && y < 192) myBmp[y][x] = color;
}

//the record sample rate
#define sample_rate  8000

//buffer to hold sound data for playback
char* sound_buffer = 0;

//buffer which is written to by the arm7
char* mic_buffer = 0;

//the length of the current data
u32 data_length = 0;

// enough to hold a screen worth of 8bit audio
u32 sound_buffer_size = 192*256;

//the mic buffer sent to the arm7 is a double buffer
//every time it is half full the arm7 signals us so we can read the
//data.  I want the buffer to swap about once per frame so i chose a
//buffer size large enough to hold two frames of 8bit mic data
u32 mic_buffer_size = sample_rate / 30;


static u32 frame = 0;

//mic stream handler
void micHandler(void* data, int length)
{
    if(!sound_buffer) return;

    if(data_length > sound_buffer_size) data_length = 0;

    DC_InvalidateRange(data, length);
    dmaCopy(data, sound_buffer + data_length, length);

    // DC_InvalidateAll ();
    // memmove(sound_buffer + data_length, data, length);
    data_length += length;
}



int main(int argc, char *argv[]) {
	int down = 0;

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
	int main = bgInit(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

	// clear the backgrounds
	memset(bgGetGfxPtr(main), 0, 256*256);

	// fill in the palette
	// gradient colors
	for(char i = -127; i < 0; i++) { 
		// Negative values are red
		u32 color_idx = (u8) i;
		BG_PALETTE[color_idx] = RGB15(-i/4, 0, 0);
	}
	for(char i = 0; i < 128; i++) { 
		u32 color_idx = i;
		BG_PALETTE[color_idx] = RGB15(i/4, i/4, i/4);
	}

	// We reserve ourselves the upper color for special purposes.
	// --> Just overriding the negative colors (out of lazyness)
	const u32 COLOR_BLACK = 0;
	const u32 COLOR_CURSOR = 127;
	BG_PALETTE[COLOR_BLACK] = RGB15(0, 0, 0);
	BG_PALETTE[COLOR_CURSOR] = RGB15(0, 31, 0);

	// Starting recording
	sound_buffer = (char*)malloc(sound_buffer_size);
	mic_buffer = (char*)malloc(mic_buffer_size);
			
	// Record
	soundMicRecord(mic_buffer, mic_buffer_size, MicFormat_8Bit, sample_rate, micHandler);

	// do stuff
	do {
		frame++;
		cpuStartTiming(0);

		if (down & KEY_R) {
		}

		if (down & KEY_L) {
		}


		if (down & KEY_DOWN) {
		}
		if (down & KEY_UP) {
		}
		if (down & KEY_LEFT) {
		}
		if (down & KEY_RIGHT) {
		}

		//for (u32 i = 0; i < data_length && i < 256*256; i++) {
		u32 y = frame % 192;
		u32 sound_offset = normalize<u32>(data_length - 256, 0, data_length);
		
		const int magnify = 5;
		for (u32 x = 0; x < 256; x++) {
			myBmp[y][x] = sound_buffer[sound_offset+x] * magnify;

			// Show current cursor
			myBmp[(y+1) % 192][x] = (char) COLOR_CURSOR;
		} 

		if (down & KEY_TOUCH) {
			const int inverse = 0;
			// FFT is done in 16bit using a temp array
			const int buffer_size = 256;
			short fft_buffer[buffer_size] = { 0 };
			for (int i = 0; i < buffer_size; i ++) {
				fft_buffer[i] = myBmp[y][i] * 4;
			}
			fix_fftr(fft_buffer, 8, inverse);
			for (int i = 0; i < buffer_size; i ++) {
				myBmp[y][i] = fft_buffer[i];
			}
			BG_PALETTE[COLOR_CURSOR] = RGB15(0, 0, 31);
		} else {
			BG_PALETTE[COLOR_CURSOR] = RGB15(0, 31, 0);
		}

		int ticks_move = cpuGetTiming();

		// wait for vblank
		int cpu_usage = REG_VCOUNT;
		swiWaitForVBlank();

		int ticks_done = cpuEndTiming();
		scanKeys();
		down = keysCurrent();

		// Print debug info
		{ 
			consoleClear(); 
			printf("cpu: %.0f%%\n", cpu_usage / 1.92f);
			printf("ticks: %d/%d (%.0f)\n", ticks_move, ticks_done, (100.0f * ticks_move / ticks_done));
			printf("data_length: %d\n", data_length);
			printf("size_of: char %d\n", sizeof(char));
		}	

		// copy from buffer to vram
		memcpy(bgGetGfxPtr(main), myBmp, sizeof(myBmp));
	  } while(true);

  return 0;
}
