// (c) LGPL 2008-2010 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

#ifdef __cplusplus
   extern "C" {
#endif
	   /**
	    * Init the gameworld. Starts the counters (frame & ms)
	    */
	void game_world_init();

	extern volatile unsigned int current_ms;
	extern volatile unsigned int current_frame;

#ifdef __cplusplus
   }
#endif

#endif // __GAME_WORLD_H__
