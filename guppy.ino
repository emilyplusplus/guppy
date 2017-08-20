#include <OneWire.h> 
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <TimeAlarms.h>

#define temp 2
#define led 3
#define button 4
#define heater 5

OneWire oneWire(temp); 
DallasTemperature sensors(&oneWire);

LiquidCrystal lcd(13,12,11,10,9,8,7);

bool ledOn = false;
bool heaterOn = false;
float current = 70.00;
float avg = 0.00;

float dataStore[20] = {0};

byte empty[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111
};

void lightsOn() {
  if(ledOn == true) return;
  
  ledOn = true;
  
  lcd.setCursor(12,0);
  lcd.print("I");
  
  for (int i = 0 ; i <= 255; i += 1) {
    analogWrite(led, i);
    delay(8);
  }
}

void lightsOff() {
  if(ledOn == false) return;
  
  ledOn = false;
  
  lcd.setCursor(12,0);
  lcd.print("O");

  for (int i = 255 ; i >= 0; i -= 1) {
    analogWrite(led, i);
    delay(8);
  }
}

void setup(void) 
{ 
  Serial.begin(9600);
  sensors.begin();
  
  setTime(14,22,0,14,8,17);

  Alarm.alarmRepeat(8,0,0,lightsOn);
  Alarm.alarmRepeat(22,0,0,lightsOff);
  
  lcd.begin(20,4);
  lcd.clear();

  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(heater, OUTPUT);

  lcd.setCursor(0,0);
  lcd.print("08:00");
  lcd.write(uint8_t(0b01111110));
  lcd.print("22:00(O) 00:00");

  lcd.setCursor(0,2);
  lcd.print("-----");
  lcd.write(uint8_t(0b11011111));
  lcd.print("F(O)");

  lcd.setCursor(0,3);
  lcd.print("-----");
  lcd.write(uint8_t(0b11011111));
  lcd.print("Favg(20h)");

  lcd.createChar(1, empty);
  lcd.setCursor(16, 3);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);
  lcd.write(1);

  Alarm.timerRepeat(5, getTemp);
  Alarm.timerRepeat(60, checkTemp);
  Alarm.timerRepeat(1, updateDisplay);
  Alarm.timerRepeat(3600ul, storeData); //60*60=3600
} 

void storeData() {
  float total = 0;
  float count = 0;
  
  for(int i = 0; i < 19; i++) {
    dataStore[i] = dataStore[i+1];
    if(dataStore[i] > 0) { total += dataStore[i]; count++; }
  }
  dataStore[19] = current;
  if(dataStore[19] > 0) { total += current; count++; }

  avg = total / count;

  if(avg > 0) {
    lcd.setCursor(0,3);
    lcd.print(avg);
  }

  for(int i = 0; i < 8; i++) {
    int top = 85;
    int bottom = 70;
    
    int pos = (i % 4) * 5;

    int raw = dataStore[pos];
    int raw2 = dataStore[pos+1];
    int raw3 = dataStore[pos+2];
    int raw4 = dataStore[pos+3];
    int raw5 = dataStore[pos+4];

    raw = min(raw, top);
    raw = max(raw, bottom);

    raw2 = min(raw2, top);
    raw2 = max(raw2, bottom);

    raw3 = min(raw3, top);
    raw3 = max(raw3, bottom);

    raw4 = min(raw4, top);
    raw4 = max(raw4, bottom);

    raw5 = min(raw5, top);
    raw5 = max(raw5, bottom);

    int t = map(raw, bottom, top, 1, 16);
    int t2 = map(raw2, bottom, top, 1, 16);
    int t3 = map(raw3, bottom, top, 1, 16);
    int t4 = map(raw4, bottom, top, 1, 16);
    int t5 = map(raw5, bottom, top, 1, 16);

    if(i < 4) {
      byte next[8] = {
        (((t == 16) ? (1) : (0)) << 4) | (((t2 == 16) ? 1 : 0) << 3) | (((t3 == 16) ? 1 : 0) << 2) | (((t4 == 16) ? 1 : 0) << 1) | (((t5 == 16) ? 1 : 0) << 0),
        (((t >= 15) ? 1 : 0) << 4) | (((t2 >= 15) ? 1 : 0) << 3) | (((t3 >= 15) ? 1 : 0) << 2) | (((t4 >= 15) ? 1 : 0) << 1) | (((t5 >= 15) ? 1 : 0) << 0),
        (((t >= 14) ? 1 : 0) << 4) | (((t2 >= 14) ? 1 : 0) << 3) | (((t3 >= 14) ? 1 : 0) << 2) | (((t4 >= 14) ? 1 : 0) << 1) | (((t5 >= 14) ? 1 : 0) << 0),
        (((t >= 13) ? 1 : 0) << 4) | (((t2 >= 13) ? 1 : 0) << 3) | (((t3 >= 13) ? 1 : 0) << 2) | (((t4 >= 13) ? 1 : 0) << 1) | (((t5 >= 13) ? 1 : 0) << 0),
        (((t >= 12) ? 1 : 0) << 4) | (((t2 >= 12) ? 1 : 0) << 3) | (((t3 >= 12) ? 1 : 0) << 2) | (((t4 >= 12) ? 1 : 0) << 1) | (((t5 >= 12) ? 1 : 0) << 0),
        (((t >= 11) ? 1 : 0) << 4) | (((t2 >= 11) ? 1 : 0) << 3) | (((t3 >= 11) ? 1 : 0) << 2) | (((t4 >= 11) ? 1 : 0) << 1) | (((t5 >= 11) ? 1 : 0) << 0),
        (((t >= 10) ? 1 : 0) << 4) | (((t2 >= 10) ? 1 : 0) << 3) | (((t3 >= 10) ? 1 : 0) << 2) | (((t4 >= 10) ? 1 : 0) << 1) | (((t5 >= 10) ? 1 : 0) << 0),
        (((t >= 9) ? 1 : 0) << 4) | (((t2 >= 9) ? 1 : 0) << 3) | (((t3 >= 9) ? 1 : 0) << 2) | (((t4 >= 9) ? 1 : 0) << 1) | (((t5 >= 9) ? 1 : 0) << 0)
      };
      lcd.createChar(i, next);
    } else {
      byte next[8] = {
        (((t >= 8) ? 1 : 0) << 4) | (((t2 >= 8) ? 1 : 0) << 3) | (((t3 >= 8) ? 1 : 0) << 2) | (((t4 >= 8) ? 1 : 0) << 1) | (((t5 >= 8) ? 1 : 0) << 0),
        (((t >= 7) ? 1 : 0) << 4) | (((t2 >= 7) ? 1 : 0) << 3) | (((t3 >= 7) ? 1 : 0) << 2) | (((t4 >= 7) ? 1 : 0) << 1) | (((t5 >= 7) ? 1 : 0) << 0),
        (((t >= 6) ? 1 : 0) << 4) | (((t2 >= 6) ? 1 : 0) << 3) | (((t3 >= 6) ? 1 : 0) << 2) | (((t4 >= 6) ? 1 : 0) << 1) | (((t5 >= 6) ? 1 : 0) << 0),
        (((t >= 5) ? 1 : 0) << 4) | (((t2 >= 5) ? 1 : 0) << 3) | (((t3 >= 5) ? 1 : 0) << 2) | (((t4 >= 5) ? 1 : 0) << 1) | (((t5 >= 5) ? 1 : 0) << 0),
        (((t >= 4) ? 1 : 0) << 4) | (((t2 >= 4) ? 1 : 0) << 3) | (((t3 >= 4) ? 1 : 0) << 2) | (((t4 >= 4) ? 1 : 0) << 1) | (((t5 >= 4) ? 1 : 0) << 0),
        (((t >= 3) ? 1 : 0) << 4) | (((t2 >= 3) ? 1 : 0) << 3) | (((t3 >= 3) ? 1 : 0) << 2) | (((t4 >= 3) ? 1 : 0) << 1) | (((t5 >= 3) ? 1 : 0) << 0),
        (((t >= 2) ? 1 : 0) << 4) | (((t2 >= 2) ? 1 : 0) << 3) | (((t3 >= 2) ? 1 : 0) << 2) | (((t4 >= 2) ? 1 : 0) << 1) | (((t5 >= 2) ? 1 : 0) << 0),
        (((t >= 1) ? 1 : 0) << 4) | (((t2 >= 1) ? 1 : 0) << 3) | (((t3 >= 1) ? 1 : 0) << 2) | (((t4 >= 1) ? 1 : 0) << 1) | (((t5 >= 1) ? 1 : 0) << 0)
      };
      lcd.createChar(i, next);
    }
  }

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
}

void getTemp() {
  sensors.requestTemperatures();
  current = sensors.getTempFByIndex(0);

  if(current > 0) {
    lcd.setCursor(0,2);
    lcd.print(current);
  }
}

void checkTemp() {
  if(current <= 0) return;
  
  if(current >= 81) {
    heaterOn = false;

    digitalWrite(heater, LOW);

    lcd.setCursor(8,2);
    lcd.print("O");
  } else if(current <= 79) {
    heaterOn = true;

    digitalWrite(heater, HIGH);

    lcd.setCursor(8,2);
    lcd.print("I");
  }
  
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
  }

  Alarm.delay(16);
} 
