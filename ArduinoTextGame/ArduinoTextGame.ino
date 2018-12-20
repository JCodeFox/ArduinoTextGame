//string.toInt();

// include the library code:
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>

const String extension=".ar";
String currentFolder="/gfiles/lsim/";
String currentFile="init";
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 8, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int cents=10000;
int currentId=0;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(10,OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  String dat=getTextFromFile("test");
  dat=getValueById(dat,"items");
  Serial.println(dat);
}

void loop() {                                      
  int val=buttonState();
  if(val>=0){
    lcd.clear(); 
    changeId(val);
    /*dat=getValue(dat,'-',val);
    dat=getValue(dat,',',0);*/
  }
}

int buttonState(){
  int num=-1;
  if(Serial.available()>0){
    String incoming = Serial.readString();
    num=incoming.toInt();
    Serial.flush();
  }
  return num;
}

int changeId(int val){
  String dat=getTextFromFile("test");
  dat=getValueById(dat,"items");
  switch(val){
    case 0:
      currentId--;
      break;
    case 1:
      dat=getValue(dat,'-',currentId);
      cents-=getValue(dat,',',1).toInt();
      break;
    case 2:
      currentId++;
      break;
    default:
      break;
  }
  dat=getValue(dat,'-',currentId);
  dat=getValue(dat,',',0);
  Serial.println(dat);
  lcd.setCursor(0,0);
  lcd.print(cents);
}

String getTextFromFile(String fileName){
    //All text stored in the file is put in this variable.
    String result="";
    File myFile=SD.open(fileName+extension);
    if (myFile) {
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      result+=char(myFile.read());
    }
      // close the file:
      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening "+fileName+extension);
    }
    return result;
}

// Copied and verified from stackoverflow
// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String getValueById(String data, String id){
  String currentData="/";
  int index=0;
  while(currentData!=""){
    currentData=getValue(data,'/',index);
    index++;
    if(getValue(currentData,':',1)==id){
      return getValue(currentData,':',2);
    }
  }
  return "";
}
