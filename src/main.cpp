#include <string>
#include <Arduino.h>
#include <EasyButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Navigation.h>
#include <BluetoothClient.h>
#include <A2DPSession.h>
#include <Menu.h>
#include <esp_pm.h>
#include "constants.h"
#include "audio_in.h"
#include "logo.h"
#include "views/HomeView.h"
#include "views/PairMenu.h"
#include "views/DevicesMenu.h"
#include "views/VolumeVisualizer.h"
#include "storage/Storage.h"

// TaoTronics TT-BA08
// --raw_addr: 11 38 117 3 197 152
BluetoothAddress testDevice(esp_bd_addr_t{11, 38, 117, 3, 197, 152});

// Desktop
// --raw_addr: 76 187 88 211 20 164
// BluetoothAddress testDevice(esp_bd_addr_t{76, 187, 88, 211, 20, 164});

#define LOG_TAG "main"

const float ADC_MODIFIER = 3.3 / 4095;
const float BATTERY_MODIFIER = 2 * (3.45 / 4095);
Adafruit_SSD1306 display(128, 32, &Wire, -1, 800000UL, 100000UL);
Navigation navigation;
// BluetoothClient bt;

EasyButton buttonA(BUTTON_A);
EasyButton buttonB(BUTTON_B);
EasyButton buttonC(BUTTON_C);

HomeView *homeView;
Menu *mainMenu;
MenuInfo *analogInputInfo;
MenuInfo *batteryInfo;
MenuInfo *cpuInfo;
VolumeVisalizer visualizer(&display);

void redraw();
void start();
void stop();
void activateBluetooth();
void turnOff();
int32_t dataCallback(uint8_t *data, int32_t len);

A2DPSession aSession(dataCallback);

GlobalTicker powerTicker(5000, []() {
  if (getBatteryPercentage() > 0.15)
  {
    if (aSession.connectionState == A2DPSession::ConnectionState::CONNECTED && aSession.mediaState == A2DPSession::MediaState::ACTIVE)
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(20));
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  else if (getBatteryPercentage() < 0.15)
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
});

GlobalTicker refreshInfo(5000, []() {
  char bV[50];
  sprintf(bV, "Battery: %.2fv", analogRead(BATTERY_PIN) * BATTERY_MODIFIER);
  batteryInfo->label = string(bV);

  char cI[50];
  sprintf(cI, "CPU: %dMHz", getCpuFrequencyMhz());
  cpuInfo->label = string(cI);
});

void setup()
{
  // Hardware
  setCpuFrequencyMhz(80);
  Serial.begin(115200);
  Serial.println("OLED FeatherWing test");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  ESP_LOGD("", "I2C Freq: %d", Wire.getClock());
  pinMode(MICROPHONE_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  // NVS & Storage
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  storage.init();

  // Display
  display.setFont(&TomThumb);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.drawBitmap(0, 0, LOGO, 128, 32, 1);
  display.display();

  // Views
  mainMenu = new Menu(&display, "Main Menu", "Main");
  homeView = new HomeView(&display, mainMenu, &visualizer, &aSession);
  navigation.navigateTo(homeView);

  // Menus
  mainMenu->custom([](Adafruit_GFX *gfx) { return new DevicesMenu(gfx); });
  mainMenu->custom([](Adafruit_GFX *gfx) { return new PairMenu(gfx); });
  mainMenu->command("Disconnect", stop, CallbackAction::Back);
  // mainMenu->command("Connect", stop);
  // mainMenu->command(
  //     "Toggle LED", []() { digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED)); });
  Menu *infoMenu = mainMenu->subMenu("Dev Inf", "Device information");
  batteryInfo = infoMenu->info("Battery: ?");
  cpuInfo = infoMenu->info("CPU: ?");
  infoMenu->command("Draw logo", []() {
    display.clearDisplay();
    display.drawBitmap(0, 0, LOGO, 128, 32, 1);
    display.display();
  });
  mainMenu->command("Turn off", []() { turnOff(); });

  // Buttons
  buttonA.begin();
  buttonB.begin();
  buttonC.begin();
  buttonA.onPressed([]() { navigation.input(KEY_A); });
  buttonB.onPressed([]() { navigation.input(KEY_B); });
  buttonC.onPressed([]() { navigation.input(KEY_C); });

  // Timers
  powerTicker.start();
  refreshInfo.start();

  // Initializations
  activateBluetooth();
  audio_init();

  aSession.start(storage.getActiveDevice().address);

  //Init finished, clear display
  vTaskDelay(pdMS_TO_TICKS(100));
  display.clearDisplay();
  display.display();
}

void redraw()
{
  // static portMUX_TYPE drawMutex = portMUX_INITIALIZER_UNLOCKED;
  display.clearDisplay();
  navigation.draw();
  display.display();
  // portENTER_CRITICAL(&drawMutex);
  // portEXIT_CRITICAL(&drawMutex);
}

void loop()
{
  GlobalTicker::updateAll();

  buttonA.read();
  buttonB.read();
  buttonC.read();

  if (navigation.needsRedraw())
    redraw();

  vTaskDelay(10);
  yield();
}

void activateBluetooth()
{
  ESP_LOGD(LOG_TAG, "Activating bluetooth...");
  if (!bluetooth.isEnabled())
  {
    bool status;
    status = bluetooth.enable();

    if (status)
    {
      ESP_LOGD(LOG_TAG, "Successfully turned on bluetooth.");
    }
    else
    {
      ESP_LOGW(LOG_TAG, "Failed to turn on bluetooth.");
      return;
    }
  }
}

void start()
{
  if (aSession.start(testDevice))
  {
    audio_start();
    // print = true;
  }
}

void stop()
{
  aSession.stop();
  vTaskDelay(pdMS_TO_TICKS(150));
  // audio_stop();
  // print = false;
}

void turnOff()
{
  stop();
  audio_stop();
  audio_deinit();
  bluetooth.disable();
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  esp_deep_sleep_start();
}

uint8_t buffer[2048];
int32_t dataCallback(uint8_t *data, int32_t len)
{
  if (len <= 0 || data == nullptr)
    return 0;

  size_t read;
  // ESP_ERROR_CHECK(i2s_read(I2S_PORT, buffer, len * 2, &read, pdMS_TO_TICKS(10)));
  ESP_ERROR_CHECK(i2s_read(I2S_PORT, buffer, len * 2, &read, portMAX_DELAY));

  for (int i = 0; i < read / 8; ++i)
  {
    data[4 * i + 0] = buffer[8 * i + 6];
    data[4 * i + 1] = buffer[8 * i + 7];
    data[4 * i + 2] = buffer[8 * i + 6];
    data[4 * i + 3] = buffer[8 * i + 7];
  }

  if (visualizer.isActive)
  {
    memcpy(visualizer.buffer, data, read / 2);
    visualizer.samples = read / 2;
  }

  return read / 2;
}
