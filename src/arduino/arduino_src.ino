#include "DHT.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define dhtPin 12
#define dhtType DHT11

DHT dht(dhtPin, dhtType);
LiquidCrystal lcd(2,3,4,5,6,7);
SoftwareSerial nodemcu(8, 9);

float humidityVal;
float tempValC;
int buzzer = 10;

int mq7pinAnalog = A1;
int mq7valueAnalog = 0;
int mq7pinDigital = 8;
int mq7valueDigital = 0;

int repetitions = 0;
int waterAlertStatus = 0;
int coAlertStatus = 0;

void setup() {
  Serial.begin(9600);
  nodemcu.begin(9600);
  dht.begin();
  lcd.begin(16,2);
}

void loop() {

  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
   
  humidityVal = dht.readHumidity();
  tempValC = dht.readTemperature();

  mq7valueAnalog = analogRead(A1) ;
  mq7valueDigital = digitalRead(mq7pinDigital);

  int rainDetectorVal = analogRead(A0);

  if(isnan(humidityVal) || isnan(tempValC)) {
    Serial.println("Reading DHT sensor failed!");
    return;
  }

  if(mq7valueAnalog >= 400 || rainDetectorVal <= 700 || tempValC <= 15 || tempValC >= 30) {
    if(mq7valueAnalog >= 400 && rainDetectorVal <= 700) {
      if(waterAlertStatus == 0 && coAlertStatus == 0) lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("HIGH CO LEVELS!");
      lcd.setCursor(0,1);
      lcd.print("WATER DETECTED!");
      coAlertStatus = 1;
      waterAlertStatus = 1;
    }else if(mq7valueAnalog >= 400) {
      if(waterAlertStatus == 0 || coAlertStatus == 0) lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("HIGH CO LEVELS!");
      coAlertStatus = 1;
      waterAlertStatus = 0;
    } else if(rainDetectorVal <= 700) {
     if(waterAlertStatus == 0 || coAlertStatus == 0) lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("WATER DETECTED!");
      waterAlertStatus = 1;
      coAlertStatus = 0;
    } 
    tone(buzzer, 1500);
    delay(500);
    noTone(buzzer);
    delay(500);
  } else {
    if(coAlertStatus == 1 || waterAlertStatus == 1) lcd.clear();
    printToLcd(tempValC, humidityVal, rainDetectorVal);
    waterAlertStatus = 0;
    coAlertStatus = 0;
  }

  printToSerial(tempValC, humidityVal, rainDetectorVal, mq7valueAnalog, mq7valueDigital);

  data["humidity"] = humidityVal;
  data["temperature"] = tempValC;
  data["rain"] = rainDetectorVal;
  data["coAnalog"] = mq7valueAnalog;
  data["coDigital"] = mq7valueDigital;
  if(repetitions == 100 || coAlertStatus == 1 || waterAlertStatus == 1) {
    data.printTo(nodemcu);
    jsonBuffer.clear();
    repetitions = 0;
  }
 
  delay(600);
  repetitions += 1;
}

void printToLcd(double tempValC, double humidityVal, double rainDetectorVal) {
  lcd.setCursor(0,0);
  lcd.print("T: ");
  lcd.print(tempValC);
  lcd.print(" C ");
  lcd.setCursor(0,1);
  lcd.print("H: ");
  lcd.print(humidityVal);
  lcd.print(" %");

}

void printToSerial(double tempValC, double humidityVal, double rainDetectorVal, double mq7valueAnalog, double mq7valueDigital) {
  Serial.print("Humidity: ");
  Serial.print(humidityVal);
  Serial.print(" %\t");

  Serial.print("Temperature: ");
  Serial.print(tempValC);
  Serial.print(" C\t");

  Serial.print("Rain detection: ");
  Serial.print(rainDetectorVal);

  Serial.print("\tCO sensor value: ");
  Serial.print(mq7valueAnalog);

  Serial.print("\tCO sensor DIGITAL: ");
  Serial.print(mq7valueDigital);

  Serial.println("");
  
}
