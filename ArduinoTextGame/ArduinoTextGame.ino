//TODO: RESET DATA READING BACK TO FILE READING!!!
const String f="items:items:burger,100-fries,200-taco,300";
const bool useSD=false;
//string.toInt();

// include the library code:
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

const String extension=".ar";
String currentFolder="/gfiles/lsim/";
String currentFile="init";
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 8, d6 = 3, d7 = 2;
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack

int cents=10000;
int currentId=0;

//stick data-------------------------------------

// Arduino Joystick pin numbers
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

// The distance the stick has to move in order to be registered as moved
const int nudge_x = 50;
const int nudge_y = 50;

// Values of stick when it is in the center
int center_x=0;
int center_y=0;
bool valueChanged=false;

void setup() {
  initStick();
  // Set the button pin to input that is high
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  lcd.begin (16,2);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.clear();
  Serial.print("Initializing SD card...");
  pinMode(10,OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    if(useSD){
      while (1);
    }
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
  /*if(Serial.available()>0){
    String incoming = Serial.readString();
    num=incoming.toInt();
    Serial.flush();
  }*/
  if((digitalRead(SW_pin)==0)&&(valueChanged==false)){
    num=1;
    valueChanged=true;
  }else if((stickYPos()>0)&&(valueChanged==false)){
    num=2;
    valueChanged=true;
  }else if((stickYPos()<0)&&(valueChanged==false)){
    num=0;
    valueChanged=true;
  }
  if((digitalRead(SW_pin)==1)&&(stickYPos()==0)){
    valueChanged=false;
  }
  return num;
}

int changeId(int val){
  String dat=getTextFromFile("test");
  dat=getValueById(dat,"items");
  switch(val){
    case 0:
      currentId--;
      if(getValue(dat,'-',currentId)==""){
        currentId++;
      }
      break;
    case 1:
      dat=getValue(dat,'-',currentId);
      cents-=getValue(dat,',',1).toInt();
      break;
    case 2:
      currentId++;
      if(getValue(dat,'-',currentId)==""){
        currentId--;
      }
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
    if(!useSD){return f;}
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
    //TODO RETURN THIS BACK TO result
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

//Stick functions------------------------
void initStick(){
  // Get the x and y pos of the stick
  center_x=analogRead(X_pin);
  center_y=analogRead(Y_pin);
  // Wait 10 miliseconds
  delay(10);
  // Calculate the average of the sticks current x and y
  // values and the values of them ten miliseconds ago.
  center_x+=analogRead(X_pin);
  center_y+=analogRead(Y_pin);
  center_x=center_x/2;
  center_y=center_y/2;
}

int stickXPos(){
  //Calculate the x position of the stick
  int raw_x=analogRead(X_pin);
  int calculated_x=raw_x-center_x;
  if((calculated_x>nudge_x)||(calculated_x<-nudge_x)){
    return calculated_x;
  }
  return 0;
}

int stickYPos(){
  //Calculate the y position of the stick
  int raw_y=analogRead(Y_pin);
  int calculated_y=raw_y-center_y;
  if((calculated_y>nudge_y)||(calculated_y<-nudge_y)){
    return -calculated_y;
  }
  return 0;
}
