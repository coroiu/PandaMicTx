#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H

#include <iostream>
#include "Menu.h"

using namespace std;

class MenuItem {
  public:
    string label;
    string name;

  protected: 
    MenuItem(string label, string name) : label(label), name(name) 
    {
    }
};

#endif