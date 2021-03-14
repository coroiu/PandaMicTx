#ifndef _DEVICE_H
#define _DEVICE_H

#include <string>
#include <DeviceInformation.h>

struct Device
{
  BluetoothAddress address;
  std::string name;
};

#endif
