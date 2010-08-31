// Copyright 2008-2009 Steve Schnepp <steve.schnepp@pwkf.org>
#ifndef __NDS_UTILS_H__
#define __NDS_UTILS_H__

#include <nds.h>

#ifdef __cplusplus
   extern "C" {
#endif

bool PA_CheckLid();
bool checkReset();

extern uint16* front;
extern uint16* back;

#ifdef __cplusplus
   }
#endif

#endif // __NDS_UTILS_H__
