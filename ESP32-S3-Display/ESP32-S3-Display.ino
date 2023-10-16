// #include "mqttPublish.h"
#include "mqttSubscribe.h"

void setup()
{
  Serial.begin(115200);
  // setupMQTTPublish();
  setupMQTTSubscribe();
}

void loop()
{
  // loopMQTTPublish();
  loopMQTTSubscribe();
}