#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "mqttBase.h"

char publishClientID[] = "publisher";

void setupMQTTPublish()
{
    mqttBaseSetup();

    // Connect to MQTT broker
    // client.setServer(mqttBroker, mqttPort);
    while (!client.connected())
    {
        connectToBroker(publishClientID);
    }
}

void loopMQTTPublish()
{
    // The timer interrupt will occur independently
    Serial.println("In Main Loop, Publishing");
    delay(1000);

    // timeloop();

    if (!client.connected())
    {
        Serial.println("Reconnecting...");
        connectToBroker(publishClientID);
    }

    // Publish data to MQTT topic
    String payload = "Hello there";
    client.publish("topic", payload.c_str());

    delay(1000); // Wait for 1 second before publishing the next message
}