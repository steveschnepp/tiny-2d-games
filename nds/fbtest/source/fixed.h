#pragma once
#ifndef FIXED_H
#define FIXED_H

// Original code from https://gist.github.com/mtheall

#include <nds.h>
#include <math.h>

class f32 {
private:
  s32 v;
public:
  f32(s32 v) : v(v) {}

  f32(float f) : v(floattof32(f)) {}

  s32 getValue() const { return v; }
  v10 tov10() const { return (v>>3)&0x3FF; }
  f32 const sqrt() const { return f32(sqrtf32(v)); }

  int getInt() const { return f32toint(v); }

  const f32 operator+(const f32 &f) const { return f32(v + f.v); }
  const f32 operator-(const f32 &f) const { return f32(v - f.v); }
  const f32 operator*(const f32 &f) const { return f32(mulf32(v, f.v)); }
  const f32 operator/(const f32 &f) const { return f32(divf32(v, f.v)); }
  const f32 operator-() const { return f32(-v); }

  f32& operator+=(const f32 &f) { v += f.v; return *this; }
  f32& operator-=(const f32 &f) { v -= f.v; return *this; }
  f32& operator*=(const f32 &f) { v = mulf32(v, f.v); return *this; }
  f32& operator/=(const f32 &f) { v = divf32(v, f.v); return *this; }

  const bool operator< (const f32 &f) const { return v < f.v; }
  const bool operator<=(const f32 &f) const { return v <= f.v; }
  const bool operator> (const f32 &f) const { return v > f.v; }
  const bool operator>=(const f32 &f) const { return v >= f.v; }
  const bool operator==(const f32 &f) const { return v == f.v; }
  const bool operator!=(const f32 &f) const { return v != f.v; }

};

#endif
