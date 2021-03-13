#ifndef _HOME_VIEW_H
#define _HOME_VIEW_H

#include <Fonts/Picopixel.h>
#include <Drawable.h>
#include <A2DPSession.h>

#define CHAR_HEIGHT 5
#define CHAR_WIDTH 5

#define SIDEBAR_WIDTH CHAR_WIDTH + 5

class HomeView : public Drawable
{
  bool hasChanged = true;
  A2DPSession *aSession;

  GFXcanvas1 sidebar;
  GFXcanvas1 canvas;

public:
  HomeView(Adafruit_GFX *gfx, A2DPSession *aSession) : Drawable(gfx), aSession(aSession), sidebar(SIDEBAR_WIDTH, gfx->height()), canvas(gfx->width() - SIDEBAR_WIDTH, gfx->height())
  {
    sidebar.setFont(&Picopixel);
    canvas.setFont(&Picopixel);
  }

  virtual bool needsRedraw()
  {
    return hasChanged;
  }

  virtual void draw()
  {
    hasChanged = false;

    drawCanvas();
    drawSidebar();

    gfx->drawBitmap(SIDEBAR_WIDTH, 0, canvas.getBuffer(), canvas.width(), canvas.height(), 1, 0);
    gfx->drawBitmap(0, 0, sidebar.getBuffer(), sidebar.width(), sidebar.height(), 1, 0);
  }

  virtual NavigationCommand *input(int key)
  {
    hasChanged = true;
    return new NopCommand();
  }

  void onEnter() override
  {
  }

  void onLeave() override
  {
  }

private:
  void drawCanvas()
  {
    // Header
    canvas.setCursor(0, CHAR_HEIGHT);
    canvas.println(":: PANDA MICROPHONE");
    canvas.drawFastHLine(0, CHAR_HEIGHT + 2, canvas.width(), 1);

    canvas.setCursor(canvas.getCursorX(), canvas.getCursorY() + 2);
    canvas.println("Connected: Desk..");
  }

  void drawSidebar()
  {
    sidebar.setCursor(0, CHAR_HEIGHT);
    sidebar.print("M");

    sidebar.setCursor(1, (sidebar.height() + CHAR_HEIGHT) / 2);
    sidebar.print("=");

    sidebar.setCursor(1, sidebar.height());
    sidebar.print("V");

    sidebar.drawFastVLine(CHAR_WIDTH + 2, 0, sidebar.height(), 1);
  }
};

#endif
