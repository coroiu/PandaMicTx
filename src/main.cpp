#include <Arduino.h>
#include <EasyButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Navigation.h>
#include <BluetoothClient.h>
#include <A2DPSession.h>
#include <esp_pm.h>
#include "PairMenu.h"
#include "audio_in.h"

// TaoTronics TT-BA08
// --raw_addr: 11 38 117 3 197 152
// BluetoothAddress testDevice(esp_bd_addr_t{11, 38, 117, 3, 197, 152});

// Desktop
// --raw_addr: 76 187 88 211 20 164
BluetoothAddress testDevice(esp_bd_addr_t{76, 187, 88, 211, 20, 164});

#define EASYBUTTON_FUNCTIONAL_SUPPORT 1
#define AUDIO_OUT_PIN DAC1
#define MICROPHONE_PIN A11
#define BATTERY_PIN A13
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14
#define AMPLIFICATION 1.5

#define LOG_TAG "main"

const float ADC_MODIFIER = 3.3 / 4095;
const float BATTERY_MODIFIER = 2 * (3.45 / 4095);
Adafruit_SSD1306 display(128, 32, &Wire);
Navigation navigation(&display);
// BluetoothClient bt;

EasyButton buttonA(BUTTON_A);
EasyButton buttonB(BUTTON_B);
EasyButton buttonC(BUTTON_C);

MenuInfo *analogInputInfo;
MenuInfo *batteryInfo;
MenuInfo *cpuInfo;

void redraw();
void startScan();
void stopScan();
void activateBluetooth();
void turnOff();
int32_t dataCallback(uint8_t *data, int32_t len);

// DiscoverySession session;
A2DPSession session(dataCallback);
// HFPSession session(dataCallback);

void setup()
{
  // setCpuFrequencyMhz(80);
  setCpuFrequencyMhz(80);
  pinMode(MICROPHONE_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  buttonA.begin();
  buttonB.begin();
  buttonC.begin();

  Serial.begin(115200);
  // Serial.begin(1000000);
  Serial.println("OLED FeatherWing test");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  display.display();
  delay(1000);

  // display.setTextSize(1);
  display.setFont(&TomThumb);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();

  Menu *mainMenu = navigation.menu();
  mainMenu->custom([](Adafruit_GFX *gfx) { return new PairMenu(gfx); });
  mainMenu->command("Start scan", startScan);
  mainMenu->command("Stop scan", stopScan);
  mainMenu->command("Toggle LED", []() { digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED)); });
  // Menu *menu1 = mainMenu->subMenu("Menu1", "Menu number 1");
  // mainMenu->subMenu("Menu2", "Menu number 2");
  // mainMenu->subMenu("Menu3", "Menu number 3");
  // mainMenu->subMenu("Menu4", "Menu number 4");
  // analogInputInfo = mainMenu->info("ADC: ?v");
  Menu *infoMenu = mainMenu->subMenu("Dev Inf", "Device information");
  batteryInfo = infoMenu->info("Battery: ?");
  cpuInfo = infoMenu->info("CPU: ?");
  mainMenu->command("Turn off", []() { turnOff(); });

  buttonA.onPressed([]() { navigation.input(KEY_UP); });
  buttonB.onPressed([]() { navigation.input(KEY_SELECT); });
  buttonC.onPressed([]() { navigation.input(KEY_DOWN); });

  activateBluetooth();
  audio_init();
}

void redraw()
{
  display.clearDisplay();
  navigation.draw();
  display.display();
}

void loop()
{
  buttonA.read();
  buttonB.read();
  buttonC.read();

  if (navigation.needsRedraw())
    redraw();

  // display.setCursor(0, 5);
  // display.print(analogRead(BATTERY_PIN) * BATTERY_MODIFIER, 2);
  // display.println("V");

  char bV[50];
  sprintf(bV, "Battery: %.2fv", analogRead(BATTERY_PIN) * BATTERY_MODIFIER);
  batteryInfo->label = string(bV);

  char cI[50];
  sprintf(cI, "CPU: %dMHz", getCpuFrequencyMhz());
  cpuInfo->label = string(cI);

  // int mic = analogRead(MICROPHONE_PIN);
  // sprintf(bV, "ADC: %.2fv", analogRead(MICROPHONE_PIN) * ADC_MODIFIER);
  // analogInputInfo->label = string(bV);

  // int mic;
  // while (1) {
  //   mic = analogRead(MICROPHONE_PIN);

  //   dacWrite(AUDIO_OUT_PIN, analogRead(MICROPHONE_PIN) / 16);
  // }

  // delay(10);
  // display.display();

  // if (print)
  // {
  //   int len = 512;
  //   size_t read;
  //   uint32_t *buff32 = (uint32_t *)buffer;
  //   uint16_t buff16[len / 2];
  //   ESP_ERROR_CHECK(i2s_read(I2S_PORT, buffer, len, &read, portMAX_DELAY));

  //   for (int i(0); i < read / 4; ++i)
  //   {
  //     buff16[i] = buff32[i] / 65536;
  //   }

  //   Serial.println(buff32[0] / 65536);
  // Serial.println(buff32[0]);
  // }

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

void startScan()
{
  ESP_LOGD(LOG_TAG, "Running test command...");

  if (session.start(testDevice))
  {
    audio_start();
    // print = true;
  }
}

void stopScan()
{
  // session.stop();
  audio_stop();
  // print = false;
}

void turnOff()
{
  bluetooth.disable();
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  esp_deep_sleep_start();
}

uint8_t buffer[2048];
int32_t dataCallback(uint8_t *data, int32_t len)
{
  size_t read;
  ESP_ERROR_CHECK(i2s_read(I2S_PORT, buffer, len * 2, &read, pdMS_TO_TICKS(10)));
  // ESP_ERROR_CHECK(i2s_read(I2S_PORT, buffer, len * 2, &read, portMAX_DELAY));

  for (int i = 0; i < read / 8; ++i)
  {
    data[4 * i + 0] = buffer[8 * i + 6];
    data[4 * i + 1] = buffer[8 * i + 7];
    data[4 * i + 2] = buffer[8 * i + 6];
    data[4 * i + 3] = buffer[8 * i + 7];
  }

  return read / 2;
}