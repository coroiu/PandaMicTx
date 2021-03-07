#include <Arduino.h>
#include <EasyButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Navigation.h>
#include <BluetoothClient.h>

// TaoTronics TT-BA08
#define EASYBUTTON_FUNCTIONAL_SUPPORT 1
#define AUDIO_OUT_PIN DAC1
#define MICROPHONE_PIN A11
#define BATTERY_PIN A13
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14

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

DiscoverySession session;

void redraw();
void startScan();
void stopScan();

void setup()
{
  pinMode(MICROPHONE_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  buttonA.begin();
  buttonB.begin();
  buttonC.begin();

  Serial.begin(9600);
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
  mainMenu->command("Start scan", startScan);
  mainMenu->command("Stop scan", stopScan);
  mainMenu->command("Toggle LED", []() { digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED)); });
  Menu *menu1 = mainMenu->subMenu("Menu1", "Menu number 1");
  mainMenu->subMenu("Menu2", "Menu number 2");
  mainMenu->subMenu("Menu3", "Menu number 3");
  mainMenu->subMenu("Menu4", "Menu number 4");
  // analogInputInfo = mainMenu->info("ADC: ?v");
  batteryInfo = mainMenu->info("Battery: ?v");

  Menu *menu11 = menu1->subMenu("M1-1", "Sub menu 1");
  menu1->subMenu("M1-2", "Sub menu 2");
  menu1->subMenu("M1-3", "Sub menu 3");
  menu1->subMenu("M1-4", "Sub menu 4");
  menu1->subMenu("M1-5", "Sub menu 5");

  menu11->subMenu("M1-1-1", "Sub sub menu 1");
  menu11->subMenu("M1-1-2", "Sub sub menu 2");
  menu11->subMenu("M1-1-3", "Sub sub menu 3");
  menu11->subMenu("M1-1-4", "Sub sub menu 4");

  buttonA.onPressed([]() { navigation.input(KEY_UP); });
  buttonB.onPressed([]() { navigation.input(KEY_SELECT); });
  buttonC.onPressed([]() { navigation.input(KEY_DOWN); });
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

  vTaskDelay(10);
  yield();
}

void startScan()
{
  ESP_LOGD(LOG_TAG, "Running test command...");

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

  // DiscoverySession session;
  session.start();
  // delay(25000);
  // session.start();
  // delay(25000);

  // delay(5000);
  // status = bluetooth.disable();

  // if (status)
  // {
  //   ESP_LOGD(LOG_TAG, "Successfully turned off bluetooth.");
  // }
  // else
  // {
  //   ESP_LOGW(LOG_TAG, "Failed to turn off bluetooth.");
  //   return;
  // }
}

void stopScan()
{
  session.stop();
}
