#ifndef _NAVIGATION_COMMAND_H
#define _NAVIGATION_COMMAND_H

#define NOP_COMMAND 0
#define BACK_COMMAND 1
#define NAVIGATE_TO_COMMAND 2

typedef int command_type_t;

class Drawable;

class NavigationCommand
{
public:
  virtual command_type_t commandType() = 0;

protected:
  NavigationCommand(){};
};

class NopCommand : public NavigationCommand
{
public:
  command_type_t commandType() override
  {
    return NOP_COMMAND;
  }
};

class BackCommand : public NavigationCommand
{
  command_type_t commandType() override
  {
    return BACK_COMMAND;
  }
};

class NavigateToCommand : public NavigationCommand
{
public:
  Drawable *drawable;

  NavigateToCommand(Drawable *drawable) : drawable(drawable){};

  command_type_t commandType() override
  {
    return NAVIGATE_TO_COMMAND;
  }
};

#endif
