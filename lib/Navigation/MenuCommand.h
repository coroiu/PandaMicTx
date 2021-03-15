#ifndef _MENU_COMMAND_H
#define _MENU_COMMAND_H

#include <functional>
#include "MenuItem.h"
#include "NavigationCommand.h"

typedef std::function<void()> callback_t;

enum CallbackAction
{
  Nop = 0,
  Back
};

class MenuCommand : public MenuItem
{
protected:
  callback_t callback;
  CallbackAction action;

public:
  MenuCommand(string label, callback_t callback, CallbackAction action) : MenuItem(label, ""), callback(callback), action(action) {}

  NavigationCommand *activate() override
  {
    callback();
    switch (action)
    {
    case CallbackAction::Nop:
      return new NopCommand();
    case CallbackAction::Back:
      return new BackCommand();
    }
  }
};

#endif
