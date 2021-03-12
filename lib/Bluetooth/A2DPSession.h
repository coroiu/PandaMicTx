#ifndef _A2DP_SESSION_H
#define _A2DP_SESSION_H

#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>
#include <esp_a2dp_api.h>
#include <esp_avrc_api.h>
#include "bluetooth.h"
#include "DeviceInformation.h"
#include "song.h";

#define BT_APP_HEART_BEAT_EVT (0xff00)

typedef void (*bt_app_cb_t)(uint16_t event, void *param);
typedef int32_t (*music_data_cb_t)(uint8_t *data, int32_t len);
// typedef int32_t (*music_data_channels_cb_t)(Channels *data, int32_t len);

/* message to be sent */
typedef struct
{
  uint16_t sig;   /*!< signal to bt_app_task */
  uint16_t event; /*!< message event id */
  bt_app_cb_t cb; /*!< context switch callback */
  void *param;    /*!< parameter area needs to be last */
} bt_app_msg_t;

typedef void (*bt_app_copy_cb_t)(bt_app_msg_t *msg, void *p_dest, void *p_src);

bool bt_app_work_dispatch(bt_app_cb_t p_cback, uint16_t event, void *p_params, int param_len, bt_app_copy_cb_t p_copy_cback);

class A2DPSession
{
  enum ConnectionState
  {
    IDLE,
    CONNECTING,
    CONNECTED
  };

  enum MediaState
  {
    APP_AV_MEDIA_STATE_IDLE,
    APP_AV_MEDIA_STATE_STARTING,
    APP_AV_MEDIA_STATE_STARTED,
    APP_AV_MEDIA_STATE_STOPPING,
  };

  static A2DPSession *ActiveSession;
  esp_a2d_connection_state_t a2dpConnectionState = ESP_A2D_CONNECTION_STATE_DISCONNECTED;
  bool avrcConnectionState = false;
  ConnectionState connectionState = ConnectionState::IDLE;
  MediaState mediaState;
  bool keepActive;
  data_callback_t dataCallback;

public:
  A2DPSession(data_callback_t dataCallback) : dataCallback(dataCallback)
  {
  }

  ~A2DPSession()
  {
    if (this == A2DPSession::ActiveSession)
    {
      this->stop();
    }
  }

  bool start(BluetoothAddress deviceAddress)
  {
    if (A2DPSession::ActiveSession != nullptr)
    {
      ESP_LOGE(DS_TAG, "Only one active a2dp session is allowed.");
      return false;
    }

    A2DPSession::ActiveSession = this;
    keepActive = true;

    // register GAP callback function
    ESP_ERROR_CHECK(esp_bt_gap_register_callback([](esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) { A2DPSession::ActiveSession->bt_gap_callback(event, param); }));

    // initialize AVRCP controller
    ESP_ERROR_CHECK(esp_avrc_ct_init());
    ESP_ERROR_CHECK(esp_avrc_ct_register_callback([](esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param) { A2DPSession::ActiveSession->bt_avrc_callback(event, param); }));

    //esp_avrc_rn_evt_cap_mask_t evt_set = {0};
    //esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_SET, &evt_set, ESP_AVRC_RN_VOLUME_CHANGE);
    //assert(esp_avrc_tg_set_rn_evt_cap(&evt_set) == ESP_OK);

    /* initialize A2DP source */
    ESP_ERROR_CHECK(esp_a2d_register_callback([](esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) { A2DPSession::ActiveSession->bt_a2dp_callback(event, param); }));
    ESP_ERROR_CHECK(esp_a2d_source_register_data_callback([](uint8_t *data, int32_t len) { return A2DPSession::ActiveSession->bt_data_callback(data, len); }));
    ESP_ERROR_CHECK(esp_a2d_source_init());

    // Set default parameters for Secure Simple Pairing
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    ESP_ERROR_CHECK(esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t)));

    // Legacy PIN
    ESP_ERROR_CHECK(esp_bt_gap_set_pin(ESP_BT_PIN_TYPE_FIXED, 0, 0));

    // // set discoverable and connectable mode
    // ESP_ERROR_CHECK(esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_NONE));
    ESP_ERROR_CHECK(esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE));
    // ESP_ERROR_CHECK(esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 20, 0));
    // ESP_ERROR_CHECK(err);
    // ESP_ERROR_CHECK(esp_bt_gap_set_cod());
    ESP_ERROR_CHECK(esp_a2d_source_connect(deviceAddress.value));

    connectionState = ConnectionState::CONNECTING;
    ESP_LOGD(DS_TAG, "Started a2dp sesssion.");
    return true;
  }

  void stop() {}

private:
  // This is how the BT Stack communicates device and connection information to us.
  void bt_gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
  {
    ESP_LOGD("", "GAP event %d", event);

    if (event == ESP_BT_GAP_DISC_RES_EVT)
    {
      ESP_LOGD("", "State: %d", param->disc_st_chg.state);
    }
    else if (event == ESP_BT_GAP_AUTH_CMPL_EVT)
    {
      if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
      {
        ESP_LOGI("", "authentication success: %s", param->auth_cmpl.device_name);
        esp_log_buffer_hex("", param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
      }
      else
      {
        ESP_LOGE("", "authentication failed, status:%d", param->auth_cmpl.stat);
      }
    }
    else if (event == ESP_BT_GAP_CFM_REQ_EVT)
    {
      ESP_LOGI("", "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
      esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
    }
  }

  // callback function for A2DP source
  void bt_a2dp_callback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
  {
    ESP_LOGD("", "A2DP event %d", event);

    if (event == ESP_A2D_CONNECTION_STATE_EVT)
    {
      a2dpConnectionState = param->conn_stat.state;
      ESP_LOGD("", "A2DP connection State: %d", a2dpConnectionState);
      if (a2dpConnectionState == ESP_A2D_CONNECTION_STATE_CONNECTED)
      {
        // a2dp_app_heart_beat(nullptr);
        ESP_ERROR_CHECK(esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_START));
        esp_bredr_tx_power_set(ESP_PWR_LVL_N12, ESP_PWR_LVL_N3);
      }
    }
    else if (event == ESP_A2D_AUDIO_STATE_EVT)
    {
      ESP_LOGD("", "A2DP audio state: %d; mcc: %d", param->audio_stat.state, param->audio_cfg.mcc);
    }
    else if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT)
    {
      ESP_LOGD("", "A2DP media ctrl cmd: %d; state: %d", param->media_ctrl_stat.cmd, param->media_ctrl_stat.status);
    }
  }

  // callback function for avrc controls
  void bt_avrc_callback(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
  {
    ESP_LOGD("", "AVRC event %d", event);
    if (event == ESP_AVRC_CT_CONNECTION_STATE_EVT)
    {
      avrcConnectionState = param->conn_stat.connected;
      ESP_LOGD("", "Avrc connection State: %d", avrcConnectionState);
    }
  }

  // callback function for audio data
  int32_t bt_data_callback(uint8_t *data, int32_t len)
  {
    static int totalPlayed = 0;
    int32_t read = dataCallback(data, len);
    // memset(data, 100, len);
    totalPlayed += read;
    if (totalPlayed % (512 * 100) == 0)
    {
      // ESP_LOGD("", "Provided data: %d", totalPlayed);
    }
    return read;
  }

  // void a2dp_app_heart_beat(void *arg)
  // {
  //   bt_app_work_dispatch(NULL, BT_APP_HEART_BEAT_EVT, NULL, 0, NULL);
  // }
};

// bool bt_app_work_dispatch(bt_app_cb_t p_cback, uint16_t event, void *p_params, int param_len, bt_app_copy_cb_t p_copy_cback)
// {
//   ESP_LOGD(BT_APP_CORE_TAG, "%s event 0x%x, param len %d", __func__, event, param_len);

//   bt_app_msg_t msg;
//   memset(&msg, 0, sizeof(bt_app_msg_t));

//   msg.sig = BT_APP_SIG_WORK_DISPATCH;
//   msg.event = event;
//   msg.cb = p_cback;

//   if (param_len == 0)
//   {
//     return bt_app_send_msg(&msg);
//   }
//   else if (p_params && param_len > 0)
//   {
//     if ((msg.param = malloc(param_len)) != NULL)
//     {
//       memcpy(msg.param, p_params, param_len);
//       /* check if caller has provided a copy callback to do the deep copy */
//       if (p_copy_cback)
//       {
//         p_copy_cback(&msg, msg.param, p_params);
//       }
//       return bt_app_send_msg(&msg);
//     }
//   }

//   return false;
// }

A2DPSession *A2DPSession::ActiveSession;

#endif
