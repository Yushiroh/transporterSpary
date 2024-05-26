#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <dht.h>

dht DHT;

#define DHT11_PIN 11
const int sprayRel = 2;
const int buzzer = 12;

const byte ROWS = 4; 
const byte COLS = 4; 

bool buzzState = false;

char hexaKeys[ROWS][COLS] = {

  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}

};
byte rowPins[ROWS] = {6, 5, 4, 3}; 
byte colPins[COLS] = {10, 9, 8, 7}; 

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long time;
unsigned long timePassed = 0;
unsigned long oneSec = 1000;

int secs = 0;
int trsh = 35;
int timeInt= 5;

bool screenCleared = false;

char customKey;
String rawVal = "";
char modeMem;

void modeA();
void modeB();
void modeC();
void modeD(int secDisp);
void modeConfirm(String key);
void sprayManager(int sprayInterval);

void setup()
{
  Serial.begin(9600);
	lcd.begin();
	lcd.backlight();
  lcd.print(" Welcome... ");
  lcd.setCursor(0,1);
  lcd.print(" Select Mode!    ");

  pinMode(sprayRel, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  delay(250);
  digitalWrite(buzzer, LOW);
  delay(250);
  digitalWrite(buzzer, HIGH);
  delay(250);
  digitalWrite(buzzer, LOW);
  delay(250);



}

void loop()
{
  
  customKey = customKeypad.getKey();
  DHT.read11(DHT11_PIN);
  // Serial.println(timeInt);
  digitalWrite(sprayRel, HIGH);

  if(DHT.temperature > trsh){

    buzzState = false;
    sprayManager(timeInt);

  }else if(DHT.temperature == trsh){

    Serial.println("Matched");  

    if(buzzState == false){
      digitalWrite(buzzer, HIGH);
      delay(50);
      digitalWrite(buzzer, LOW);
      delay(50);
      digitalWrite(buzzer, HIGH);
      delay(50);
      digitalWrite(buzzer, LOW);
      delay(50);
      digitalWrite(buzzer, HIGH);
      delay(50);
      digitalWrite(buzzer, LOW);
      delay(50);
      buzzState = true;
    }else{
      sprayManager(timeInt);
    }

  }else{
    buzzState = false;
    Serial.println("Temp not matching!");
  }

  if (customKey){
  
    if(customKey == 'A'){
      Serial.println("Mode A");
      rawVal = "";
      modeMem = 'A';
      lcd.clear();
      modeA();
    }else if(customKey == 'B'){
      Serial.println("Mode B");
      rawVal = "";
      modeMem = 'B';
      lcd.clear();
      modeB();
    }else if(customKey == 'C'){
      Serial.println("Mode C");
      rawVal = "";
      modeMem = 'C';
      lcd.clear();
      modeC();
    }else if(customKey == 'D'){

      rawVal = "";
      modeMem = 'D';
      lcd.clear();
      modeD(secs);

    }else if(customKey == '*'){
      Serial.println("Mode *");
      modeConfirm(rawVal);
    }else if(customKey == '#'){
      Serial.println("Mode #");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Cancelled!");
      delay(1000);
      lcd.clear();
      modeA();    
    }else{
      lcd.print(customKey);
      rawVal += customKey;
      Serial.println(rawVal);
    }

  }

  // lcd.setCursor(0,1);
  // lcd.print("Temp= ");
  // lcd.print(DHT.temperature);

  // time = millis();

  // if(time - timePassed >= oneSec){
  // secs++;
  // lcd.setCursor(1,0);
  // lcd.print(secs);
  // timePassed = time;
  // }

}

void modeA(){
  //Display Parameters (Spray Interval, Temperature Threshold, Current Temp)

  lcd.setCursor(0,0);
  lcd.print("Tsl:");
  lcd.print(trsh);
  lcd.print("C Int:");
  lcd.print(timeInt);
  lcd.print("m");

  lcd.setCursor(0,1);
  lcd.print("LiveTemp:");
  lcd.print(DHT.temperature);
  lcd.print("C   ");

  //Trsl:00C Int:00m
  //LiveTemp:00.00C
}

void modeB(){
  //Set spray Threshold
  lcd.setCursor(0,0);
  lcd.print("Spray Int (mins)");
  lcd.setCursor(0,1);
  lcd.print("Value: ");
  lcd.setCursor(7,1);
}

void modeC(){
  //Set Temperature Threshold
  lcd.setCursor(0,0);
  lcd.print("Temp treshold(C)");
  lcd.setCursor(0,1);
  lcd.print("Value: ");
  lcd.setCursor(7,1);
}

void modeD(int secDisp){
  //Set Temperature Threshold
  lcd.setCursor(0,0);
  lcd.print("Timer Count");
  lcd.setCursor(0,1);
  lcd.print("Value: ");
}

void modeConfirm(String key){

  int valueSel = key.toInt();

  if(valueSel >= 61 and modeMem == 'B'){

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Value too High!");
    delay(1000);
    modeA();

  }else if(valueSel >= 99 and modeMem == 'C'){

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Value too High!");
    delay(1000);
    modeA();

  }else if(valueSel < 60 and modeMem == 'B'){

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Interval Set!");
    lcd.setCursor(0,1);
    lcd.print("Value: ");
    lcd.print(valueSel);
    timeInt = valueSel;
    delay(1000);
    modeA();

  }else if(valueSel < 99 and modeMem == 'C'){

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp Set!");
    lcd.setCursor(0,1);
    lcd.print("Value: ");
    lcd.print(valueSel);
    trsh = valueSel;
    delay(1000);
    modeA();

  }

}

void sprayManager(int sprayInterval){

  int sprayIntInSecs = sprayInterval * 60;

  time = millis();

  if(time - timePassed >= oneSec){
  secs++;
  timePassed = time;
  }

  if(sprayIntInSecs == secs){

    digitalWrite(buzzer, HIGH);
    digitalWrite(sprayRel, LOW);   
    delay(3000);
    digitalWrite(buzzer, LOW);
    digitalWrite(sprayRel, HIGH);
    rawVal = "";
    modeMem = 'A';
    lcd.clear();
    modeA();
    secs = 0;

  }

  Serial.println(secs);
  if(modeMem == 'D'){
    lcd.setCursor(7,1);
    lcd.print(secs);
  }


}

void sprayNotif(){
  
}

