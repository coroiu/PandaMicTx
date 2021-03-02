#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Navigation.h>

#define BATTERY_PIN A13
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14

const float BATTERY_MODIFIER = 2 * (3.45 / 4095);
Adafruit_SSD1306 display(128, 32, &Wire);
Navigation navigation(&display);

void setup()
{
  pinMode(BATTERY_PIN, INPUT);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(BUILTIN_LED, OUTPUT);

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

  Menu* mainMenu = navigation.menu();

  Menu* menu1 = mainMenu->subMenu("Menu1", "Menu number 1");

  Menu* menu2 = mainMenu->subMenu("Menu2", "Menu number 2");
}

void loop()
{
  // if (!digitalRead(BUTTON_A))
  // {
  //   digitalWrite(BUILTIN_LED, HIGH);
  //   // menu.up();
  // }
  // else
  // {
  //   digitalWrite(BUILTIN_LED, LOW);
  // }

  if (!digitalRead(BUTTON_A))
  {
    navigation.input(KEY_UP);
  }
  else if (!digitalRead(BUTTON_C))
  {
    navigation.input(KEY_DOWN);
  }
  
  display.clearDisplay();

  navigation.draw();

  // display.setCursor(0, 5);
  // display.print(analogRead(BATTERY_PIN) * BATTERY_MODIFIER, 2);
  // display.println("V");

  delay(10);
  display.display();
}
