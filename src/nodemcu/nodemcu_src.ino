#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"
#include "discord.h"

SoftwareSerial nodemcu(D6, D5);

const long coAlertInterval = 60000;
unsigned long coAlertPreviousMillis = 0;

const long waterAlertInterval = 60000;
unsigned long waterAlertPreviousMillis = 0;

const long infoInterval = 60000;
unsigned long infoPreviousMillis = 0;

void setup() {
  Serial.begin(9600);
  nodemcu.begin(9600);
  while(!Serial) continue;
  connectWIFI();
  delay(1000);
}

void loop() {
  unsigned long coAlertCurrentMillis = millis();
  unsigned long waterCurrentMillis = coAlertCurrentMillis;
  unsigned long infoCurrentMillis = coAlertCurrentMillis;
 
  
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu);
  
  if (data == JsonObject::invalid()) {
    Serial.println("Invalid Json Object");
    jsonBuffer.clear();
    return;
  }
  float humidityVal = data["humidity"];
  float tempValC = data["temperature"];
  int rainDetectorVal = data["rain"];
  int mq7valueAnalog = data["coAnalog"];
  int mq7valueDigital = data["coDigital"];

  if(mq7valueAnalog >= 400){
    if(coAlertCurrentMillis - coAlertPreviousMillis >= coAlertInterval) {
      coAlertPreviousMillis = coAlertCurrentMillis;
      sendDiscordAlert("co");
    }
  }

  if(rainDetectorVal <= 700) {
    if(waterCurrentMillis - waterAlertPreviousMillis >= waterAlertInterval) {
      waterAlertPreviousMillis = waterCurrentMillis;
      sendDiscordAlert("water");
    }
  }

  if(infoCurrentMillis - infoPreviousMillis >= infoInterval) {
    infoPreviousMillis = infoCurrentMillis;
    sendDiscordData(humidityVal, tempValC, rainDetectorVal);
  }
  printToSerial(tempValC,  humidityVal,  rainDetectorVal,  mq7valueAnalog, mq7valueDigital);
 
}

void sendDiscordData(float humidityVal, float tempValC, int rainDetectorVal) {
    String data = "Temperatura: " + String(tempValC, 1) + " C " + "Wilgotność: " + String(humidityVal, 1) + " % " + "Deszcz: " + String(rainDetectorVal);
    sendDiscord(data, "info");
}

void sendDiscordAlert(String type) {
  String alert;
  if (type == "co") {
    alert = "WARNING: HIGH CO LEVELS!";
  } else if (type == "water") {
    alert = "WARNING: WATER DETECTED ON THE SENSOR";
  }
  sendDiscord(alert, "alert");
}

void printToSerial(double tempValC, double humidityVal, double rainDetectorVal, double mq7valueAnalog, double mq7valueDigital) {
  
  Serial.println("JSON object received");
  
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
