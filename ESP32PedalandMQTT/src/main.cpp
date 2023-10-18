#include <Arduino.h>
#include <stdio.h>
#include "pedalCheck.h"
#include "testOnBoardLED.h"
#include "testAmplifier.h"
#include "wifiTest.h"
#include "mqttPublish.h"
#include "mqttSubscribe.h"

void setup()
{
  Serial.begin(115200);
  // setupLED();
  // setupChannelRead();
  setupPedalCheck();
  // setupWifiTest();

  // setupMQTTPublish();
  // setupMQTTSubscribe();
}

void loop()
{
  // loopLED();
  // loopSendPWM();
  loopPedalCheck();
  // loopWifiTest();

  // loopMQTTPublish();
  // loopMQTTSubscribe();
}

// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json