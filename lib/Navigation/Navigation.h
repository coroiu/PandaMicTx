#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include "Menu.h"
#include "MenuItem.h"
#include "MenuCommand.h"

class Navigation
{
  Adafruit_GFX *gfx;
  Menu mainMenu;

public:
  Navigation(Adafruit_GFX *gfx) : gfx(gfx), mainMenu(gfx, "Main Menu", "Main")
  {
  }

  Menu* menu() 
  {
    return &mainMenu;
  }

  void draw()
  {
    mainMenu.draw();
  }

  void input(int key)
  {
    mainMenu.input(key);
  }
};

#endif