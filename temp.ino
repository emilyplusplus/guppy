#include <OneWire.h> 
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <TimeAlarms.h>

#define temp 2
#define led 3
#define button 4

OneWire oneWire(temp); 
DallasTemperature sensors(&oneWire);

LiquidCrystal lcd(13,12,11,10,9,8,7);

bool ledOn = true;
float current = 75.00;

byte customChar[8] = {
  0b01000,
  0b11100,
  0b11110,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte customChar2[8] = {
  0b00000,
  0b00000,
  0b01000,
  0b11110,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte customChar3[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10000,
  0b11111,
  0b11111
};

byte customChar4[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

void lightsOn() {
  for (int i = 0 ; i <= 255; i += 1) {
    analogWrite(led, i);
    delay(8);
  }
}

void lightsOff() {
  for (int i = 255 ; i >= 0; i -= 1) {
    analogWrite(led, i);
    delay(8);
  }
}

void setup(void) 
{ 
  Serial.begin(9600);
  sensors.begin(); 
  setTime(1,52,0,4,8,17);

  Alarm.alarmRepeat(9,0,0,lightsOn);
  Alarm.alarmRepeat(23,0,0,lightsOff);
  
  lcd.begin(20,4);
  lcd.clear();

  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  lcd.setCursor(0,0);
  lcd.print("09:00");
  lcd.write(uint8_t(0b01111110));
  lcd.print("23:00(I) 22:20");

  lcd.setCursor(19,1);
  lcd.print("!");

  lcd.setCursor(0,2);
  lcd.print(75.45);
  lcd.write(uint8_t(0b11011111));
  lcd.print("F -+"); //F +2.34

  lcd.setCursor(0,3);
  lcd.print("-----");
  lcd.write(uint8_t(0b11011111));
  lcd.print("Favg(20h)");

  lcd.createChar(0, customChar);
  lcd.createChar(1, customChar2);
  lcd.createChar(2, customChar3);
  lcd.createChar(3, customChar3);
  lcd.createChar(4, customChar4);
  lcd.createChar(5, customChar4);
  lcd.createChar(6, customChar4);
  lcd.createChar(7, customChar4);
  lcd.setCursor(16, 2);
  lcd.write((byte)0);
  lcd.write(1);
  lcd.write(2);
  lcd.write(3);
  lcd.setCursor(16, 3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(6);
  lcd.write(7);

  Alarm.timerRepeat(30, getTemp);
  Alarm.timerRepeat(1, updateDisplay);

  lightsOn();
} 

void getTemp() {
  sensors.requestTemperatures();
  current = sensors.getTempFByIndex(0);
}

void updateDisplay() {
  lcd.setCursor(15,0);
  
  if(hour() < 10) {
    lcd.print("0");
  }
  lcd.print(hour());
  
  lcd.print(":");
  
  if(minute() < 10) {
    lcd.print("0");
  }
  lcd.print(minute());
}

void loop(void) 
{
  if(digitalRead(button) == LOW) {
    if(ledOn == true) {
      lightsOff();
    } else {
      lightsOn();
    }

    ledOn = !ledOn;
  }

  Alarm.delay(16);
} 
