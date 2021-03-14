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
    ESP_LOGD("", "Setting active device: %s (%s)", device.name, device.address.toString().c_str());
    data.activeDevice = device;
    commit();
  }

  std::vector<Device> getDevices()
  {
    return std::vector<Device>(data.devices, data.devices + data.count);
  }

  void addDevice(Device device)
  {
    ESP_LOGD("", "Adding new device: %s (%s)", device.name, device.address.toString().c_str());
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
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_get_u32(_nvs_handle, "version", &version));
    ESP_LOGD("", "Load Storage: Current version is %d - found %d.", VERSION, version);
    if (version != VERSION)
      return;

    uint32_t length = sizeof(Data);
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_get_blob(_nvs_handle, "data", &data, &length));
  }

  void commit()
  {
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_set_u32(_nvs_handle, "version", VERSION));
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_set_blob(_nvs_handle, "data", &data, sizeof(Data)));
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_commit(_nvs_handle));
  }
} storage;

extern Storage storage;

#endif
