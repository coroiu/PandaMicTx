#ifndef _PAIR_COMMAND_H
#define _PAIR_COMMAND_H

#include <MenuCommand.h>
#include "storage/Storage.h"

class PairCommand : public MenuCommand
{
  DeviceInformation device;

public:
  PairCommand(string label, DeviceInformation device)
      : MenuCommand(
            label, []() {}, CallbackAction::Nop),
        device(device)
  {
  }

  NavigationCommand *activate() override
  {
    Device d{.address = device.address};
    device.name.copy(d.name, sizeof(d.name));
    ESP_LOGD("", "PRESSED: %s (%s)", device.name.c_str(), device.address.toString().c_str());
    storage.addDevice(d);
    storage.setActiveDevice(d);
    return new BackCommand();
  }
};

#endif
