# arduino-CO-detector

A project of a device monitoring levels of carbon monoxide gas in the air, measuring temperature and humidity and detecting if water has been splashed on the sensor. Could be used as a safety device in a boiler room.
<br>
Based on Arduino Uno for gathering data and NodeMCU v3 for sending data to Discord.

## Description

This device sends information about the current temperature and humidity levels in a room to a chosen channel on a Discord server. It also displays this information on an LCD screen. In case of CO concentration in the air rising to a high level, having detected water, temperature dropping low or rising too high, the device sends an alert to the same (or different) channel and keeps sending alerts with a set interval between them. Moreover, a buzzer starts beeping and information about the alert is displayed on the LCD screen.

## Modules and sensors used in the project

- Arduino Uno
- carbon monoxide detector - MQ-7 Waveshare 9532 (5V)
- temperature and humidity sensor - DHT-11 (5V)
- water detector - YL-83 (5V)
- NodeMCU v3 with ESP8266 (3.3V)
- LCD 2x16 (5V)
- buzzer (5V)

## Schematic

<img width="612" alt="schematic" src="https://user-images.githubusercontent.com/92263066/171998535-638b4c93-8d98-4f13-9d8f-7c43c1b1d1ea.png">


