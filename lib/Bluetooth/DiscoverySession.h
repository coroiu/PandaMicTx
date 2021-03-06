#ifndef _DISCOVERY_SESSION_H
#define _DISCOVERY_SESSION_H

#include <string>
#include <map>
#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>
#include "BluetoothClient.h"
#include "DeviceInformation.h"

#define DS_TAG

// #define SCAN_MODE ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE
#define SCAN_MODE ESP_BT_SCAN_MODE_NONE

class DiscoverySession
{
  static DiscoverySession *ActiveSession;
  bool keepActive;

public:
  std::function<void(DeviceInformation)> callback;
  std::map<string, DeviceInformation> devices;

  DiscoverySession()
  {
  }

  ~DiscoverySession()
  {
    if (this == DiscoverySession::ActiveSession)
    {
      this->stop();
    }
  }

  void onNewDevice(std::function<void(DeviceInformation)> callback)
  {
    this->callback = callback;
  }

  bool start()
  {
    if (DiscoverySession::ActiveSession != nullptr)
    {
      ESP_LOGE(DS_TAG, "Only one active discovery session is allowed.");
      return false;
    }

    DiscoverySession::ActiveSession = this;
    keepActive = true;

    // register GAP callback function
    esp_err_t err;
    err = esp_bt_gap_register_callback([](esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) { DiscoverySession::ActiveSession->bt_api_callback(event, param); });
    ESP_ERROR_CHECK(err);

    // set discoverable and connectable mode
    err = esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_NONE);
    ESP_ERROR_CHECK(err);

    // This function starts Inquiry and Name Discovery
    startInquiry();

    ESP_LOGD(DS_TAG, "Started discovery sesssion.");
    return true;
  }

  bool stop()
  {
    if (DiscoverySession::ActiveSession != this)
    {
      ESP_LOGE(DS_TAG, "Cannot stop discovery session that is not active.");
      return false;
    }

    DiscoverySession::ActiveSession = nullptr;
    keepActive = false;
    esp_err_t err = esp_bt_gap_cancel_discovery();
    ESP_ERROR_CHECK(err);

    ESP_LOGD(DS_TAG, "Stopped discovery sesssion.");
    return true;
  }

private:
  void startInquiry()
  {
    // This function starts Inquiry and Name Discovery
    esp_err_t err = esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 20, 0);
    ESP_ERROR_CHECK(err);
  }

  // This is how the BT Stack communicates information to us.
  void bt_api_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
  {
    // ESP_LOGD(DS_TAG, "Received event %d", event);

    if (event == ESP_BT_GAP_DISC_RES_EVT)
    {
      char bda_str[18];
      // esp_bd_addr_t address;
      BluetoothAddress address;
      uint32_t cod = 0;
      int rssi = -129; /* invalid value */
      uint8_t bdname_len;
      char bdname[ESP_BT_GAP_MAX_BDNAME_LEN];
      uint8_t eir[ESP_BT_GAP_EIR_DATA_LEN];
      uint8_t eir_len;
      esp_bt_gap_dev_prop_t *p;

      address = param->disc_res.bda;

      // ESP_LOGI(GAP_TAG, "Device found: %s", bda2str(param->disc_res.bda, bda_str, 18));

      if (devices.count(address.toString()) == 1)
      {
        // ESP_LOGI(GAP_TAG, "Found an old device again.");
        return;
      }

      ESP_LOGI(GAP_TAG, "Device found: %s", address.toString().c_str());
      // for (int i = 0; i < param->disc_res.num_prop; i++)
      // {
      //   p = param->disc_res.prop + i;
      //   switch (p->type)
      //   {
      //   case ESP_BT_GAP_DEV_PROP_COD:
      //     cod = *(uint32_t *)(p->val);
      //     ESP_LOGI(GAP_TAG, "--Class of Device: 0x%x", cod);
      //     break;
      //   case ESP_BT_GAP_DEV_PROP_RSSI:
      //     rssi = *(int8_t *)(p->val);
      //     ESP_LOGI(GAP_TAG, "--RSSI: %d", rssi);
      //     break;
      //   case ESP_BT_GAP_DEV_PROP_BDNAME:
      //   default:
      //     break;
      //   }
      // }

      /* search for device with MAJOR service class as "rendering" in COD */
      // app_gap_cb_t *p_dev = &m_dev_info;
      // if (p_dev->dev_found && 0 != memcmp(param->disc_res.bda, p_dev->bda, ESP_BD_ADDR_LEN))
      // {
      //   return;
      // }

      // if (!esp_bt_gap_is_valid_cod(cod) ||
      //     !(esp_bt_gap_get_cod_major_dev(cod) == ESP_BT_COD_MAJOR_DEV_PHONE))
      // {
      //   return;
      // }

      // memcpy(p_dev->bda, param->disc_res.bda, ESP_BD_ADDR_LEN);
      // p_dev->dev_found = true;
      for (int i = 0; i < param->disc_res.num_prop; i++)
      {
        p = param->disc_res.prop + i;
        switch (p->type)
        {
        case ESP_BT_GAP_DEV_PROP_COD:
          cod = *(uint32_t *)(p->val);
          ESP_LOGI(GAP_TAG, "--Class of Device: 0x%x", cod);
          break;
        case ESP_BT_GAP_DEV_PROP_RSSI:
          rssi = *(int8_t *)(p->val);
          ESP_LOGI(GAP_TAG, "--RSSI: %d", rssi);
          break;
        case ESP_BT_GAP_DEV_PROP_BDNAME:
        {
          bdname_len = (p->len > ESP_BT_GAP_MAX_BDNAME_LEN) ? ESP_BT_GAP_MAX_BDNAME_LEN : (uint8_t)p->len;
          memcpy(&bdname, (uint8_t *)(p->val), bdname_len);
          bdname[bdname_len] = '\0';
          ESP_LOGD(GAP_TAG, "--? name: %s", bdname);
          break;
        }
        case ESP_BT_GAP_DEV_PROP_EIR:
        {
          eir_len = p->len;
          memcpy(&eir, (uint8_t *)(p->val), eir_len);
          break;
        }
        default:
          break;
        }
      }

      bool name_res = get_name_from_eir(eir, bdname, &bdname_len);
      if (!name_res)
        return;

      ESP_LOGD(GAP_TAG, "--name: %s", bdname);

      DeviceInformation deviceInfo = {
          .address = address,
          .name = bdname,
          .rssi = rssi};
      devices[deviceInfo.address.toString()] = deviceInfo;
    }
    else if (event == ESP_BT_GAP_DISC_STATE_CHANGED_EVT)
    {
      if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED)
      {
        if (keepActive)
        {
          ESP_LOGD(GAP_TAG, "Restarting device discovery.");
          startInquiry();
        }
        else
        {
          ESP_LOGD(GAP_TAG, "Device discovery stopped.");
        }
      }
    }

    static char *bda2str(esp_bd_addr_t bda, char *str, size_t size)
    {
      if (bda == NULL || str == NULL || size < 18)
      {
        return NULL;
      }

      uint8_t *p = bda;
      sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
              p[0], p[1], p[2], p[3], p[4], p[5]);
      return str;
    }

    // Try to extract device name from the Bluetooth Extended Inquiry Response.
    static bool get_name_from_eir(uint8_t * eir, char *bdname, uint8_t *bdname_len)
    {

      uint8_t *rmt_bdname = NULL;
      uint8_t rmt_bdname_len = 0;

      if (!eir)
      {
        return false;
      }

      // Try to get long name
      rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
      if (!rmt_bdname)
      {
        // Try to get short name
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
      }

      if (rmt_bdname)
      {
        // If the name is larger that the max (not sure why this length)
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN)
        {
          rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }

        // If we were provided with an output variable
        if (bdname)
        {
          // Copy the name from the EIR to the variable, making sure to null-terminate it.
          memcpy(bdname, rmt_bdname, rmt_bdname_len);
          bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len)
        {
          // Also output the length
          *bdname_len = rmt_bdname_len;
        }
        return true;
      }

      return false;
    }
  };

  DiscoverySession *DiscoverySession::ActiveSession;

#endif
