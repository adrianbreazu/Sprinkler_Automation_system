// ---------------------------------------------------------------------------
// Created by Breazu Adrian - breazuadrian@gmail.com
// Copyright 2015 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
// used DHT library, written by ladyada
// used RTC sample, taken from http://www.elecrow.com/wiki/index.php?title=Tiny_RTC
// ---------------------------------------------------------------------------

#include "DHT.h"
#include "Wire.h"

// define device and pin numbers
#define Temperature_Pin 2
#define Button_Pin 3
#define Relay_Roses_Pin 10
#define Relay_Back_Pin 11
#define Relay_BBQ_Pin 12
#define Relay_Plants_Pin 13
#define RTC_ADDRESS 0x68

//initialize temperature sensor
DHT dht(Temperature_Pin, DHT11);

//RTC global variables
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

// button state
int PrevButtonState;

//debug mode
boolean DebugMode = true;

//set default state to true
boolean IrrigationDay = true;

// set default button state
int ButtonState;

void setup () {
  Serial.begin(9600);

  //set pin device mode
  pinMode(Relay_Roses_Pin, OUTPUT);
  pinMode(Relay_Back_Pin, OUTPUT);
  pinMode(Relay_BBQ_Pin, OUTPUT);
  pinMode(Relay_Plants_Pin, OUTPUT);
  pinMode(Button_Pin, INPUT);
  
  // initialize HDT sensor
  if (DebugMode)
    Serial.println("temperature init");
  dht.begin();
  
  // initialize RTC
  Wire.begin();
  //initialize RTC date and time
  //setDateDs1307(00, 00, 20, 3, 24, 6, 15);
}

// Convert normal decimal numbers to binary coded decimal
byte DecToBcd(byte val) {
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte BcdToDec(byte val) {
  return ( (val/16*10) + (val%16) );
}

void SetDebugMode(boolean value) {
  DebugMode = value;
}

// Function to set the currnt time, change the second&minute&hour to the right time
void SetDateDs1307(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(DecToBcd(0));
  Wire.write(DecToBcd(second));    // 0 to bit 7 starts the clock
  Wire.write(DecToBcd(minute));
  Wire.write(DecToBcd(hour));      // If you want 12 hour am/pm you need to set
                                  // bit 6 (also need to change readDateDs1307)
  Wire.write(DecToBcd(dayOfWeek));
  Wire.write(DecToBcd(dayOfMonth));
  Wire.write(DecToBcd(month));
  Wire.write(DecToBcd(year));
  Wire.endTransmission();
}

// Function to gets the date and time from the ds1307 and prints result
void GetDateDs1307() {
  // Reset the register pointer
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(DecToBcd(0));
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDRESS, 7);
  
  second     = BcdToDec(Wire.read() & 0x7f);
  minute     = BcdToDec(Wire.read());
  hour       = BcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
  dayOfWeek  = BcdToDec(Wire.read());
  dayOfMonth = BcdToDec(Wire.read());
  month      = BcdToDec(Wire.read());
  year       = BcdToDec(Wire.read());  

  if (DebugMode) {
    //write the time and date
    Serial.print(hour, DEC);
    Serial.print(":");
    Serial.print(minute, DEC);
    Serial.print(":");
    Serial.print(second, DEC);
    Serial.print("  ");
    Serial.print(month, DEC);
    Serial.print("/");
    Serial.print(dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(year,DEC);
    Serial.print("  ");
    Serial.println();
  }
}

void ButtonPressed (boolean btn_state) {
  if (btn_state) {
    if (DebugMode)
      Serial.println("Button pressed.");
    digitalWrite(Relay_Roses_Pin, HIGH);
    digitalWrite(Relay_Back_Pin, LOW);
    digitalWrite(Relay_BBQ_Pin, LOW);
    digitalWrite(Relay_Plants_Pin, LOW);
  }
  else
    WaterArea(0,1);
}

void WaterArea (int area, int time) {
  int water_time = 0;
  
  // Water the corresponding area
  //water_time = time * 60*1000;

  // 1 sec for testing purpose
  water_time = time * 1000;

  if (DebugMode) {
    Serial.print("Water area ");
    Serial.print(area);
    Serial.print(" for ");
    Serial.print(water_time);
    Serial.println("miliseconds");
  }
  
  switch (area) {
    case 0:
      digitalWrite(Relay_Roses_Pin, LOW);
      digitalWrite(Relay_Back_Pin, LOW);
      digitalWrite(Relay_BBQ_Pin, LOW);
      digitalWrite(Relay_Plants_Pin, LOW);
      delay(5000);
      break;
    case 1:
      digitalWrite(Relay_Roses_Pin, HIGH);
      delay(water_time);
      digitalWrite(Relay_Roses_Pin, LOW);
      break;
    case 2:
      digitalWrite(Relay_Back_Pin, HIGH);
      delay(water_time);
      digitalWrite(Relay_Back_Pin, LOW);
      break;
    case 3:
      digitalWrite(Relay_BBQ_Pin, HIGH);
      delay(water_time);
      digitalWrite(Relay_BBQ_Pin, LOW);
      break;
    case 4:
      digitalWrite(Relay_Plants_Pin, HIGH);
      delay(water_time);
      digitalWrite(Relay_Plants_Pin, LOW);
      break;    
    default:
      digitalWrite(Relay_Roses_Pin, LOW);
      digitalWrite(Relay_Back_Pin, LOW);
      digitalWrite(Relay_BBQ_Pin, LOW);
      digitalWrite(Relay_Plants_Pin, LOW);
      break;
  }
}

float  GetHumidity() {
  // wait before read
  delay(2000);

  return dht.readHumidity();
}

float  GetTemperature() {
  return dht.readTemperature();
}

void loop() {
  float humidity, temperature;

  GetDateDs1307();

  humidity = GetHumidity();
  temperature = GetTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    if (DebugMode)
      Serial.println("Temperature read failed");
  }
  else {
    if (DebugMode) {
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print(" | Temprature: ");
      Serial.println(temperature);
    }
  }
    
  //read button state
  delay(2000);
  ButtonState = digitalRead(Button_Pin);
  if (DebugMode) {
    Serial.print("button state ");
    Serial.print(ButtonState);
    Serial.print(" previous state ");
    Serial.println(PrevButtonState);
  }
  
  // check for the button status
  if (ButtonState != PrevButtonState) {
    if (ButtonState == HIGH)
      ButtonPressed(true);
    else
      ButtonPressed(false);
  }

  // set the previous button state
  PrevButtonState = ButtonState;
  
  //check for irrigation time 
  if (hour == 3 && minute == 00) {
    if (DebugMode) {
      Serial.print("Time to water ");
      Serial.println(IrrigationDay);
    }
    
    if (IrrigationDay) {
      // disable irrigation tomorrow
      IrrigationDay = false;
      
      if (DebugMode)
        Serial.print("Irrigation time today.");

      if (DebugMode) {
        Serial.println("Irrigation mode debug.");
        WaterArea (1,1);
        WaterArea (2,1);
        WaterArea (3,1);
        WaterArea (4,1);  
        //delay (55000);
      }
      else {
        WaterArea (1,10);
        WaterArea (2,7);
        WaterArea (3,7);
        WaterArea (4,5);  
      }
    }
    else {
      IrrigationDay = true; // enable irrigation tomorrow      
      if (DebugMode)
        Serial.println("Irrigation time tomorrow.");
    }
  }
  else
  {
    if (DebugMode)
      Serial.println("Not yet time to water.");
    WaterArea (0,1);
  }
}
