#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "mqttBase.h"

char subscribeClientID[] = "driverDisplay";

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String incomingMessage;

  for (int i = 0; i < length; i++)
  {
    // Serial.print(i);
    // Serial.print((char)message[i]);
    incomingMessage += (char)message[i];
  }
  Serial.println();
  Serial.printf("Message again: %s", incomingMessage);
}

void setupMQTTSubscribe()
{
  Serial.println("Before base setup");
  mqttBaseSetup();
  Serial.println("After base setup");

  // Connect to MQTT broker
  while (!client.connected())
  {
    Serial.println("Trying to connect to broker");
      connectToBroker(subscribeClientID);
  }
  // Serial.println("BEFORE setting callback");
  client.setCallback(callback);
  // Serial.println("AFTER setting callback");
}

void loopMQTTSubscribe()
{
  // The timer interrupt will occur independently
  Serial.println("In Main Loop, subscribed");
  delay(1000);

  // timeloop();

  if (!client.connected())
  {
    Serial.println("Reconnecting...");
    connectToBroker(subscribeClientID);
  }

  delay(1000); // Wait for 1 second before publishing the next message
}