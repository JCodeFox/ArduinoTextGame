
// include the library code:
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int cents=10000;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("$");
  lcd.print(cents/100);
}

void loop() {
  // put your main code here, to run repeatedly:

}
