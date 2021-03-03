#include <Arduino.h>
#include <EasyButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Navigation.h>
#include <BluetoothA2DPSource.h>

// TaoTronics TT-BA08
#define EASYBUTTON_FUNCTIONAL_SUPPORT 1
#define AUDIO_OUT_PIN DAC1
#define MICROPHONE_PIN A11
#define BATTERY_PIN A13
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14

const float ADC_MODIFIER = 3.3 / 4095;
const float BATTERY_MODIFIER = 2 * (3.45 / 4095);
Adafruit_SSD1306 display(128, 32, &Wire);
Navigation navigation(&display);

EasyButton buttonA(BUTTON_A);
EasyButton buttonB(BUTTON_B);
EasyButton buttonC(BUTTON_C);

MenuInfo *analogInputInfo;
MenuInfo *batteryInfo;

void testConnect();

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
  Menu *menu1 = mainMenu->subMenu("Menu1", "Menu number 1");
  mainMenu->command("Toggle LED", []() { digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED)); });
  mainMenu->command("Connect to test device", testConnect);
  // analogInputInfo = mainMenu->info("ADC: ?v");
  batteryInfo = mainMenu->info("Battery: ?v");

  buttonA.onPressed([]() { navigation.input(KEY_UP); });
  buttonB.onPressed([]() { navigation.input(KEY_SELECT); });
  buttonC.onPressed([]() { navigation.input(KEY_DOWN); });
}

void loop()
{
  buttonA.read();
  buttonB.read();
  buttonC.read();

  display.clearDisplay();

  navigation.draw();

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

  delay(10);
  display.display();
}

void testConnect() {

}