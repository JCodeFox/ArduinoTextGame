
// include the library code:
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

const String extension=".txt";
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int cents=10000;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("$");
  lcd.print(cents/100);
}

void loop() {
  // put your main code here, to run repeatedly:

}

String getTextFromFile(String fileName){
    //All text stored in the file is put in this variable.
    String result="";
    File myFile=SD.open(fileName+extension);
    if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      result=result+myFile.read();
    }
      // close the file:
      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening "+fileName+extension);
    }
    return result;
}
