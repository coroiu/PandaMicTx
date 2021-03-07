#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include <stack>
#include "Menu.h"
#include "MenuItem.h"
#include "MenuCommand.h"
#include "NavigationCommand.h"

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

  bool needsRedraw()
  {
    return currentMenu()->needsRedraw();
  }

  void draw()
  {
    currentMenu()->draw();
  }

  void input(int key)
  {
    NavigationCommand *command = currentMenu()->input(key);

    if (command->commandType() == BACK_COMMAND)
    {
      back();
    }
    else if (command->commandType() == NAVIGATE_TO_COMMAND)
    {
      NavigateToCommand *navigateToCommand = static_cast<NavigateToCommand *>(command);
      navigateTo(navigateToCommand->menu);
    }

    delete command;
  }

  void navigateTo(Menu *menu)
  {
    stack.push(menu);
    menu->onEnter();
  }

  void back()
  {
    currentMenu()->onLeave();
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
