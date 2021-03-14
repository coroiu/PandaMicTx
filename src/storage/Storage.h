#ifndef _STORAGE_H
#define _STORAGE_H

#include <nvs.h>
#include "Device.h"

#define VERSION 1
#define MAX_DEVICES 30

struct Data
{
  Device activeDevice;
  size_t count = 0;
  Device devices[MAX_DEVICES];
};

class Storage
{
  Data data;
  nvs_handle _nvs_handle;

public:
  Storage()
  {
  }

  void init()
  {
    ESP_ERROR_CHECK(nvs_open("panda-mic", NVS_READWRITE, &_nvs_handle));
    read();
  }

  Device getActiveDevice()
  {
    return data.activeDevice;
  }

  void setActiveDevice(Device device)
  {
    data.activeDevice = device;
    commit();
  }

  std::vector<Device> getDevices()
  {
    return std::vector<Device>(data.devices, data.devices + data.count);
  }

  void addDevice(Device device)
  {
    data.devices[data.count++] = device;
    commit();
  }

  void removeDevice(size_t index)
  {
    std::copy(data.devices + index + 1,  // copy everything starting here
              data.devices + data.count, // and ending here, not including it,
              data.devices + index);     // to this destination
    --data.count;
    commit();
  }

private:
  void read()
  {
    uint32_t version = 0;
    nvs_get_u32(_nvs_handle, "version", &version);
    if (version != VERSION)
      return;

    nvs_get_blob(_nvs_handle, "data", &data, nullptr);
  }

  void commit()
  {
    nvs_set_blob(_nvs_handle, "data", &data, sizeof(Data));
    nvs_set_u32(_nvs_handle, "version", VERSION);
  }
} storage;

extern Storage storage;

#endif
