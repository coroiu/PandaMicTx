#ifndef _DRAWABLE_H
#define _DRAWABLE_H

#include <Adafruit_GFX.h>
#include <NavigationCommand.h>

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_SELECT 2

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
