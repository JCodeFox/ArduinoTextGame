//TODO: RESET DATA READING BACK TO FILE READING!!!
const String f="items:items:burger,1-fries,2-taco,3";
const bool useSD=true;
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
LiquidCrystal  lcd(rs, en, d4, d5, d6, d7);

int health=100;
int dollars=100;
int currentId=0;

//stick data-------------------------------------

// Arduino Joystick pin numbers
const int SW_pin = 9; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
const int menus=2;

// The distance the stick has to move in order to be registered as moved
const int nudge_x = 50;
const int nudge_y = 50;

// Values of stick when it is in the center
int center_x=0;
int center_y=0;
bool valueChanged=false;
int menu=1;
int frame=0;

void setup() {
  initStick();
  // Set the button pin to input that is high
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  lcd.begin(16, 2);
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
  writeTextToFile("data/inv","DATA1");
  writeTextToFile("data/inv","DATA2");
  writeTextToFile("data/inv","DATA3");
  Serial.println(getTextFromFile("data/inv"));
}

void loop() {                            
  int val=buttonState();
  if(val>=0){
    lcd.clear();
    if(menu==0){ 
      changeId(val);
    }else if(menu==1){
      healthScreen(val);
    }
    /*dat=getValue(dat,'-',val);
    dat=getValue(dat,',',0);*/
  }
  frame+=1;
  if(frame>=500){
    health-=1;
    frame=0;
    if(menu==1){
      healthScreen(-1);
    }else if(menu==0){
      changeId(-1);
    }
  }
  delay(1);
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
  if((stickXPos()>0)&&(valueChanged==false)){
    menu+=1;
    valueChanged=true;
  }else if((stickXPos()<0)&&(valueChanged==false)){
    menu-=1;
    valueChanged=true;
  }
  if((digitalRead(SW_pin)==1)&&(stickYPos()==0)){
    valueChanged=false;
  }
  if(menu<0){
    menu=0;
  }else if(menu>=menus){
    menu=menus-1;
  }
  return num;
}
//Show health and money
int healthScreen(int val){
  if(val==0){
    dollars+=1;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("$");
  lcd.print(dollars);
  lcd.setCursor(0,1);
  lcd.print(health);
}
int changeId(int val){
  String dat=getTextFromFile("test");
  dat=getValueById(dat,"items");
  if(val==0){
      currentId--;
      if(currentId<0){
        currentId=0;
      }
  }else if(val==1){
    String item=getValue(dat,'-',currentId);
    dollars-=getValue(item,',',1).toInt();
    health+=5;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Paid:");
    lcd.print(getValue(item,',',1));
    lcd.setCursor(0,1);
    lcd.print(getValue(item,',',0));
    delay(1000);
    lcd.clear();
  }else if(val==2){
    currentId++;
    if(getValue(dat,'-',currentId)==""){
      currentId--;
    }
  }
  dat=getValue(dat,'-',currentId);
  String itemName=getValue(dat,',',0);
  String price=getValue(dat,',',1);
  lcd.setCursor(0,0);
  lcd.print(dollars);
  lcd.setCursor(0,1);
  lcd.print(itemName);
  lcd.print(":"+price);
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
void writeTextToFile(String fileName, String data){
  File myFile = SD.open(fileName+extension,FILE_WRITE);
  myFile.println(data);
  myFile.close();
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
