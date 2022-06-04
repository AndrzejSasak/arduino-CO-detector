#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
const char ssid[] = SECRET_SSID;    // name of WiFi hotspot
const char pass[] = SECRET_PASS;    // password
const String discord_info_webhook = SECRET_INFO_WEBHOOK; //Discord webhook link for the channel with standard info
const String discord_alert_webhook = SECRET_ALERT_WEBHOOK; //Discord webhook link for the channel with alert info
const String discord_info_tts = SECRET_INFO_TTS;
const String discord_alert_tts = SECRET_ALERT_TTS;
ESP8266WiFiMulti WiFiMulti;
WiFiClientSecure client;

void connectWIFI() {
  WiFiMulti.addAP(ssid, pass);
  WiFi.mode(WIFI_STA);
  client.setInsecure();
  Serial.print("[WiFi] Connecting to: ");
  Serial.println(ssid);
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.println("[WiFi] Connected");
}

void sendDiscord(String content, String channel) {
  String discord_webhook;
  String discord_tts;
  if(channel == "alert") { //choosing alert webhook
    discord_webhook = discord_alert_webhook;
    discord_tts = discord_alert_tts;
  }
  if(channel == "info") { //choosing info webhook
    discord_webhook = discord_info_webhook;
    discord_tts = discord_info_tts;
  }
  
  HTTPClient https;
  Serial.println("[HTTP] Connecting to Discord...");
  Serial.println("[HTTP] Message: " + content);
  Serial.println("[HTTP] TTS: " + discord_tts);
  
  if (https.begin(client, discord_webhook)) {  // beginning HTTPS connection
    // adding HTTP header
    https.addHeader("Content-Type", "application/json");
    // using HTTP POST method
    int httpCode = https.POST("{\"content\":\"" + content + "\",\"tts\":" + discord_tts + "}");

    if (httpCode > 0) { //HTTP response code greater than 0 on success
      Serial.print("[HTTP] Status code: ");
      Serial.println(httpCode);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String response = https.getString();
        Serial.print("[HTTP] Response: ");
        Serial.println(response);
      }
    } else {
      Serial.print("[HTTP] Post... failed, error: ");
      Serial.println(https.errorToString(httpCode).c_str());
    }
    https.end();
  } else {
    Serial.printf("[HTTP] Unable to connect\n");
  }
}
