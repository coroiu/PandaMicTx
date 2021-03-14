#ifndef _STORAGE_H
#define _STORAGE_H

#include <ArduinoJson.h>
#include <ArduinoNvs.h>
#include "Device.h"

#define MAX_DEVICES 30

class Storage
{
  DynamicJsonDocument document;
  ArduinoNvs nvs;

public:
  Storage() : document(1024)
  {
    nvs.begin("panda-mic");

    if (!deserialize())
    {
      init();
    }
  }

  std::vector<Device> getDevices()
  {
    std::vector<Device> toReturn;
    for (JsonVariant v : document["devices"].to<JsonArray>())
    {
      toReturn.push_back(Device{
          .address = v["address"].as<std::string>(),
          .name = v["name"].as<std::string>()});
    }

    return toReturn;
  }

  void addDevice(Device device)
  {
    JsonObject json;
    json["address"] = device.address.toString();
    json["name"] = device.name;
    document["devices"].add(json);
    serialize();
  }

private:
  void init()
  {
    document.createNestedArray("devices");
  }

  bool deserialize()
  {
    uint8_t output[1024];
    if (nvs.getBlob("configuration", output, 1024))
      return false;

    deserializeMsgPack(document, (const uint8_t *)output);
  }

  bool serialize()
  {
    uint8_t output[1024];
    size_t size = serializeMsgPack(document, output, 1024);
    nvs.setBlob("configuration", output, size);
  }

} storage;

extern Storage storage;

#endif
