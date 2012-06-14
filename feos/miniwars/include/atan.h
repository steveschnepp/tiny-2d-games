#pragma once
#ifndef ATAN_H
#define ATAN_H

#ifdef FEOS
#include <feos.h>
#else
#include <nds.h>
#endif

#define BRAD_PI (1<<14)

u32 atan2Tonc(s32 y, s32 x) {
  if(y == 0)
    return (x >= 0 ? 0 : BRAD_PI);

  static const int fixShift = 15;
  int phi = 0, t, t2, dphi;

  if(y <  0) {          x =  -x; y = -y; phi += 4; }
  if(x <= 0) { t =   x; x =   y; y = -t; phi += 2; }
  if(x <= y) { t = y-x; x = x+y; y =  t; phi += 1; }
  phi *= BRAD_PI/4;

  t = div64((s64)y<<fixShift, x);
  t2 = -t*t>>fixShift;

  dphi = 0x0470;
  dphi = 0x1029 + (t2*dphi>>fixShift);
  dphi = 0x1F0B + (t2*dphi>>fixShift);
  dphi = 0x364C + (t2*dphi>>fixShift);
  dphi = 0xA2FC + (t2*dphi>>fixShift);
  dphi = dphi*t>>fixShift;

  return phi + ((dphi+4)>>3);
}

#endif /* ATAN_H */

