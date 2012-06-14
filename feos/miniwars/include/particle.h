#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

class Particle {
protected:
  s32  x;
  s32  y;
  bool valid;
public:
  Particle(s32 x, s32 y) : x(x), y(y), valid(true) {}
  virtual ~Particle() {}

	virtual void move()    = 0;
  virtual s32  getX()    = 0;
  virtual s32  getY()    = 0;
  virtual bool isValid() = 0;
};

#endif /* PARTICLE_H */

