#ifndef _MENU_COMMAND_H
#define _MENU_COMMAND_H

#include <functional>
#include "MenuItem.h"
#include "NavigationCommand.h"

typedef std::function<void()> callback_t;

class MenuCommand : public MenuItem
{
  callback_t callback;

public:
  MenuCommand(string label, callback_t callback) : MenuItem(label, ""), callback(callback) {}

  NavigationCommand *activate()
  {
    callback();
    return new NopCommand();
  }
};

#endif
