#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include <stack>
#include "Menu.h"
#include "MenuItem.h"
#include "MenuCommand.h"
#include "NavigationCommand.h"

class Navigation
{
  std::stack<Drawable *> stack;

public:
  Navigation()
  {
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
    if (stack.size() > 0)
      currentFrame()->onLeave();
    stack.push(drawable);
    drawable->onEnter();
  }

  void back()
  {
    currentFrame()->onLeave();
    stack.pop();
    currentFrame()->onEnter();
  }

private:
  Drawable *currentFrame()
  {
    return stack.top();
  }
};

#endif
