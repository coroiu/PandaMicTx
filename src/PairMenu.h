#ifndef _PAIR_MENU_H
#define _PAIR_MENU_H

#include <DiscoverySession.h>
#include <Menu.h>

class PairMenu : public Menu
{
  DiscoverySession session;

public:
  PairMenu(Adafruit_GFX *gfx) : Menu(gfx, "Scan for new device", "Pairing")
  {
    session.onNewDevice([&](DeviceInformation device) { this->info(device.name + " (" + device.address.toString() + ")"); });
  }

  void onEnter() override
  {
    session.start();
  }

  void onLeave() override
  {
    session.stop();
    session.clear();
    clear();
  }
};

#endif
