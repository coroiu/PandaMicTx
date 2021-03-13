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
  std::stack<Drawable *> stack;

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
    return currentFrame()->needsRedraw();
  }

  void draw()
  {
    currentFrame()->draw();
  }

  void input(int key)
  {
    NavigationCommand *command = currentFrame()->input(key);

    if (command->commandType() == BACK_COMMAND)
    {
      back();
    }
    else if (command->commandType() == NAVIGATE_TO_COMMAND)
    {
      NavigateToCommand *navigateToCommand = static_cast<NavigateToCommand *>(command);
      navigateTo(navigateToCommand->drawable);
    }

    delete command;
  }

  void navigateTo(Drawable *drawable)
  {
    stack.push(drawable);
    drawable->onEnter();
  }

  void back()
  {
    currentFrame()->onLeave();
    stack.pop();
  }

private:
  Drawable *currentFrame()
  {
    if (stack.size() == 0)
      return &mainMenu;
    else
      return stack.top();
  }
};

#endif
