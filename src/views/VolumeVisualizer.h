#ifndef _VOLUME_VISUALIZER_H
#define _VOLUME_VISUALIZER_H

#include <Adafruit_GFX.h>
#include <Drawable.h>

class VolumeVisalizer : public Drawable
{
public:
  GlobalTicker refreshTicker;
  int32_t samples = 0;
  uint8_t buffer[4096];
  bool isActive = false;
  double volume = 0;
  bool hasChanged = false;

  VolumeVisalizer(Adafruit_GFX *gfx)
      : Drawable(gfx),
        refreshTicker(
            17, [&]() { calculateVolume(); }, 0, MICROS)
  {
    refreshTicker.start();
  }

  virtual bool needsRedraw()
  {
    return hasChanged;
  }

  virtual void draw()
  {
    int width = gfx->width() * volume;
    gfx->fillScreen(0);
    gfx->fillRect(gfx->width() - width, 0, width, gfx->height(), 1);
    // canvas.setCursor(1, 5);
    // canvas.println(volume);
    //
    // gfx->drawBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height(), 1);
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

private:
  void calculateVolume()
  {
    if (isActive && samples > 0)
    {
      int32_t monoSamples = samples / 2;
      int16_t *data16 = (int16_t *)buffer;
      double minVal(1e8);
      double maxVal(-1e8);
      double mean(0);

      for (int i(0); i < monoSamples; i += 2)
      {
        mean += data16[i];
      }
      mean /= monoSamples;

      for (int i(0); i < monoSamples; i += 2)
      {
        minVal = min(minVal, data16[i] - mean);
        maxVal = max(maxVal, data16[i] - mean);
      }

      double dB = log10((maxVal - minVal) / 32768) * 20; // SPL
      volume = max((dB + 65.0) / 65.0, 0.0);
      samples = 0;
      hasChanged = true;
    }
  }
};

#endif
