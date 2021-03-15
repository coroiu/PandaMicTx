#ifndef _VOLUME_VISUALIZER_H
#define _VOLUME_VISUALIZER_H

#include <Adafruit_GFX.h>
#include <Drawable.h>

class VolumeVisalizer : public Drawable
{
public:
  bool isActive = false;
  double volume = 0;

  VolumeVisalizer(Adafruit_GFX *gfx) : Drawable(gfx) {}

  virtual bool needsRedraw()
  {
    return true;
  }

  virtual void draw()
  {
    gfx->drawRect(0, 0, gfx->width() * volume, gfx->height(), 1);
    gfx->setCursor(10, 10);
    gfx->println(volume);
  }

  virtual NavigationCommand *input(int key)
  {
    return new BackCommand();
  }

  virtual void onEnter()
  {
    isActive = true;
  }

  virtual void onLeave()
  {
    isActive = false;
  }
};

#endif
