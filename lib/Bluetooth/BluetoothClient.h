#ifndef _BLUETOOTH_CLIENT_H
#define _BLUETOOTH_CLIENT_H

#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "DiscoverySession.h"

#define BC_TAG
#define BT_DEVICE_NAME "PandaMic"

class BluetoothClient
{
  DiscoverySession *discoverySession;

public:
  bool isEnabled()
  {
    return esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED;
  }

  bool enable()
  {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_UNINITIALIZED)
    {
      ESP_LOGW(BC_TAG, "Bluetooth already enabled (Status: %d)", esp_bluedroid_get_status());
      return false;
    }

    // Release memory used by BLE. Not sure why, but hey
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    // bt_cfg.auto_latency = true;
    // bt_cfg.mode = ESP_BT_MODE_BTDM;
    // bt_cfg.mode = ESP_BT_MODE_CLASSIC_BT;

    // Turn on bluetooth
    btStart();
    ESP_LOGD(BC_TAG, "btStarted() said: %d \n", btStarted());

    if (esp_bluedroid_init() != ESP_OK)
    {
      ESP_LOGE(BC_TAG, "%s initialize bluedroid failed\n", __func__);
      return false;
    }
    ESP_LOGD(BC_TAG, "esp_bluedroid_get_status() after esp_bluedroid_init() said: %d \n", esp_bluedroid_get_status());

    if (esp_bluedroid_enable() != ESP_OK)
    {
      ESP_LOGE(BC_TAG, "%s enable bluedroid failed\n", __func__);
      return false;
    }
    ESP_LOGD(BC_TAG, "esp_bluedroid_get_status() after esp_bluedroid_enable() said: %d \n", esp_bluedroid_get_status());

    esp_bt_dev_set_device_name(BT_DEVICE_NAME);

    return true;
    // Entry point into application
    // bt_app_gap_start_up();
  }

  bool disable()
  {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED)
    {
      ESP_LOGW(BC_TAG, "Bluetooth already disabled (Status: %d)", esp_bluedroid_get_status());
      return false;
    }

    if (esp_bluedroid_disable() != ESP_OK)
    {
      ESP_LOGE(BC_TAG, "%s disable bluedroid failed\n", __func__);
      return false;
    }
    ESP_LOGD(BC_TAG, "esp_bluedroid_get_status() after esp_bluedroid_disable() said: %d \n", esp_bluedroid_get_status());

    if (esp_bluedroid_deinit() != ESP_OK)
    {
      ESP_LOGE(BC_TAG, "%s initialize bluedroid failed\n", __func__);
      return false;
    }
    ESP_LOGD(BC_TAG, "esp_bluedroid_get_status() after esp_bluedroid_init() said: %d \n", esp_bluedroid_get_status());

    return true;
  }
} bluetooth;

extern BluetoothClient bluetooth;

#endif
