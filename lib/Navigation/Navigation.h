#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include <stack>
#include "Menu.h"
#include "MenuItem.h"
#include "MenuCommand.h"

class Navigation
{
  Menu mainMenu;
  std::stack<Menu *> stack;

public:
  Adafruit_GFX *gfx;

  Navigation(Adafruit_GFX *gfx) : gfx(gfx), mainMenu(gfx, "Main Menu", "Main")
  {
  }

  Menu *menu()
  {
    return &mainMenu;
  }

  void draw()
  {
    currentMenu()->draw();
  }

  void input(int key)
  {
    currentMenu()->input(key);
  }

  void navigateTo(Menu *menu)
  {
    stack.push(menu);
  }

  void back()
  {
    stack.pop();
  }

private:
  Menu *currentMenu()
  {
    if (stack.size() == 0)
      return &mainMenu;
    else
      return stack.top();
  }
};

#endif
