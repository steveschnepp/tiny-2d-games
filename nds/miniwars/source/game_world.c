// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#include "game_world.h"

#include <nds.h>

volatile unsigned int current_ms = 0;
volatile unsigned int current_frame = 0;

/** 
 * Handle the current_ms counter, will be called at 1kHz 
 */
void timing_intr_handler() {
	++current_ms;
}

// XXX - This one is never called. The update is done 
// directly from main to be able to use swiWaitForVBlank();
void frame_intr_handler() {
	++current_frame;
}

void game_world_init() {
	// Set divisor and IRQ req for timer number 2
	TIMER_CR(1) = TIMER_ENABLE | TIMER_IRQ_REQ | TIMER_DIV_1024;
	TIMER_DATA(1) = TIMER_FREQ_1024(1000); // 1kHz frequency
	irqSet(IRQ_TIMER1, timing_intr_handler);
	irqEnable(IRQ_TIMER1);

//	irqSet(IRQ_VBLANK, frame_intr_handler);
//	irqEnable(IRQ_VBLANK);
}
