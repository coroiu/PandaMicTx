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

  Device getActiveDevice()
  {
    return mapToDevice(document["activeDevice"]);
  }

  void setActiveDevice(Device device)
  {
    document["activeDevice"] = mapToJson(device);
    serialize();
  }

  std::vector<Device> getDevices()
  {
    std::vector<Device> toReturn;
    for (JsonVariant v : document["devices"].to<JsonArray>())
    {
      toReturn.push_back(mapToDevice(v));
    }

    return toReturn;
  }

  void addDevice(Device device)
  {
    document["devices"].add(mapToJson(device));
    serialize();
  }

private:
  void init()
  {
    document["activeDevice"] = mapToJson(Device());
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

  Device mapToDevice(JsonVariant v)
  {
    return Device{
        .address = v["address"].as<std::string>(),
        .name = v["name"].as<std::string>()};
  }

  JsonObject mapToJson(Device d)
  {
    JsonObject json;
    json["address"] = d.address.toString();
    json["name"] = d.name;
    return json;
  }
} storage;

extern Storage storage;

#endif
