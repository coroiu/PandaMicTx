#ifndef _MENU_COMMAND_H
#define _MENU_COMMAND_H

#include "MenuItem.h"

class MenuCommand : public MenuItem
{
public:
  MenuCommand(string label) : MenuItem(label, "") {}
};

#endif