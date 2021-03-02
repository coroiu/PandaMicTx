#ifndef _MENU_H
#define _MENU_H

#include <iostream>
#include <Adafruit_GFX.h>
#include <Fonts/TomThumb.h>
#include "Menu.h"
#include "MenuItem.h"
#include "MenuCommand.h"

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_SELECT 2

#define CHAR_HEIGHT 5
#define CHAR_WIDTH 3
#define LINE_WIDTH 1
#define SPACING 1

#define HEADER_WIDTH (CHAR_HEIGHT + LINE_WIDTH + SPACING)

using namespace std;

class Menu : public MenuItem
{
  Adafruit_GFX *gfx;
  GFXcanvas1 headerCanvas;
  GFXcanvas1 mainCanvas;

  int selectedItem;
  vector<MenuItem*> menuItems;

public:
  Menu(Adafruit_GFX *gfx, string label) : Menu(gfx, label, ""){};

  Menu(Adafruit_GFX *gfx, string label, string name) : gfx(gfx), headerCanvas(HEADER_WIDTH, gfx->height()), mainCanvas(gfx->width() - HEADER_WIDTH, gfx->width()), MenuItem(label, name)
  {
    headerCanvas.setFont(&TomThumb);
    mainCanvas.setFont(&TomThumb);
    command("Back");
  }

  void draw()
  {
    if (!name.empty())
    {
      drawHeader();
      drawCanvasToGfx(&headerCanvas, 0, 0);
    }

    drawMenu();
    drawCanvasToGfx(&mainCanvas, name.empty() ? 0 : headerCanvas.width() + CHAR_WIDTH, 0);
  }

  void input(int key)
  {
    if (key == KEY_UP)
    {
      selectedItem = (selectedItem - 1) % menuItems.size();
    }
    else if (key == KEY_DOWN)
    {
      selectedItem = (selectedItem + 1) % menuItems.size();
    }
  }

  Menu *subMenu(string name, string label)
  {
    Menu *menu = new Menu(gfx, label, name);
    menuItems.push_back(menu);
    return menu;
  }

  void command(string label) 
  {
    MenuCommand *command = new MenuCommand(label);
    menuItems.push_back(command);
  }

private:
  void drawHeader()
  {
    headerCanvas.setRotation(3);
    headerCanvas.setCursor(0, CHAR_HEIGHT);
    headerCanvas.print(name.c_str());
    headerCanvas.drawLine(0, CHAR_HEIGHT + SPACING, headerCanvas.width(), CHAR_HEIGHT + SPACING, 1);
    headerCanvas.setRotation(0);
  }

  void drawMenu()
  {
    mainCanvas.setCursor(0, CHAR_HEIGHT);

    for (int i(0); i < menuItems.size(); ++i)
    {
      MenuItem* item = menuItems[i];

      if (i == selectedItem) 
      {
        mainCanvas.print("> ");
      } 
      else 
      {
        mainCanvas.setCursor(mainCanvas.getCursorX() + CHAR_WIDTH * 2, mainCanvas.getCursorY());
      }

      mainCanvas.println(item->label.c_str());
    }
    mainCanvas.println(selectedItem);
  }

  void drawCanvasToGfx(GFXcanvas1 *canvas, int x, int y)
  {
    gfx->drawBitmap(x, y, canvas->getBuffer(), canvas->width(), canvas->height(), 1, 0);
  }
};

#endif