#ifndef _MENU_INFO_H
#define _MENU_INFO_H

#include "MenuItem.h"

class MenuInfo : public MenuItem
{
public:
  MenuInfo(string label) : MenuItem(label, "") {}

  NavigationCommand *activate()
  {
    return new NopCommand();
  }
};

#endif
