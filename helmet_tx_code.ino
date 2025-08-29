#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include "DHT.h"

// Define LCD pins
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Define sensor objects
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Explicitly define analog pins
#ifndef A2
#define A2 16  // Assign A2 if not defined
#endif

#ifndef A3
#define A3 17  // Assign A3 if not defined
#endif

#define DHTPIN A2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define sensor and actuator pins
int gas = A0;
int g1 = 7;
int g2 = A3;  // Ensure A3 is correctly recognized
int buz = 3;
int m1 = 5;
int m2 = 6;
int cnt = 0;

void setup() {
  Serial.begin(9600);
  
  // Initialize sensors
  dht.begin();
  if (!accel.begin()) {
    Serial.println("ADXL345 not detected. Check connections.");
    while (1);
  }

  // Set pin modes
  pinMode(buz, OUTPUT);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(gas, INPUT);
  pinMode(g1, INPUT);
  pinMode(g2, INPUT);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("  WELCOME");
  delay(1500);
  lcd.clear();
}

void loop() {
  cnt++;
  
  // Read sensors
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int gval = analogRead(gas);
  int Lval = analogRead(g2);
  int hel = digitalRead(g1);

  sensors_event_t event;
  accel.getEvent(&event);
  int xval = event.acceleration.x;
  int yval = event.acceleration.y;

  // Display values on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:" + String(t) + " H:" + String(h) + " G:" + String(gval));
  lcd.setCursor(0, 1);
  lcd.print("hs:" + String(hel) + " x:" + String(xval) + " y:" + String(yval));
  delay(1000);

  // Alarm and motor control logic
  if (gval > 380 || hel == 1 || xval < -3) {
    digitalWrite(buz, HIGH);
    Serial.println(String(t) + "," + String(h) + "," + String(gval) + "," + 
                   String(yval) + "," + String(xval) + "," + String(Lval));
  } 
  else if (t > 40 || h > 90) {
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
  } 
  else {
    digitalWrite(m1, LOW);
    digitalWrite(m2, LOW);
    digitalWrite(buz, LOW);
  }

  // Print sensor values to Serial Monitor
  Serial.println(String(t) + "," + String(h) + "," + String(gval) + "," + 
                 String(yval) + "," + String(xval) + "," + String(Lval));
}
