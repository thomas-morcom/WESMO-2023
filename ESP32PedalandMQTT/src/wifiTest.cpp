#include <Arduino.h>
#include <stdio.h>

// #include <WiFiMulti.h>
#include <WiFi.h>

#define WIFI_SSID "mywifi"
#define WIFI_PASSWORD "1234567890"

// WiFiMulti wiFiMulti;

void setupWifiTest()
{
    pinMode(LED_BUILTIN, OUTPUT);

    // wiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
    // while (wiFiMulti.run() != WL_CONNECTED)
    // {
    //   delay(100);
    // }
    // Serial.print("Connected\n");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("starting\n");
}

bool isConnected = false;

void loopWifiTest()
{
    // digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);
    if (WiFi.status() == WL_CONNECTED && !isConnected)
    {
        Serial.print("Connected\n");
        digitalWrite(LED_BUILTIN, HIGH);
        isConnected = true;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(1000);
        isConnected = false;
    }
}
