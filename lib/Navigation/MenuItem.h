#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H

#include <iostream>
#include "Menu.h"
#include "NavigationCommand.h"

using namespace std;

class MenuItem
{
public:
  string label;
  string name;

  virtual NavigationCommand *activate() = 0;

protected:
  MenuItem(string label, string name) : label(label), name(name)
  {
  }
};

#endif
