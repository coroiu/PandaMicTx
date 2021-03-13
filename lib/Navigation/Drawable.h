#ifndef _DRAWABLE_H
#define _DRAWABLE_H

#include <Adafruit_GFX.h>
#include <NavigationCommand.h>

#define KEY_A 0
#define KEY_B 1
#define KEY_C 2

#define KEY_UP KEY_A
#define KEY_SELECT KEY_B
#define KEY_DOWN KEY_C

class Drawable
{
protected:
  Adafruit_GFX *gfx;

public:
  Drawable(Adafruit_GFX *gfx) : gfx(gfx) {}

  virtual bool needsRedraw() = 0;
  virtual void draw() = 0;

  virtual NavigationCommand *input(int key)
  {
    return new NopCommand();
  }

  virtual void onEnter() = 0;
  virtual void onLeave() = 0;
};

#endif
