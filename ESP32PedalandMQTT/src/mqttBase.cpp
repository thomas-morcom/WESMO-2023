#include <Arduino.h>
#include <stdio.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "mywifi";
const char *password = "1234567890";
const char *mqttBroker = "172.20.10.4";
// const int mqttPort = 1883;
int mqttPort;
// const char *mqttClientID = "subscribe";

hw_timer_t *timer = NULL;

WiFiClient espClient;
PubSubClient client(espClient);

void IRAM_ATTR timerISR()
{
    Serial.println("Timer interrupt occurred!");
    for (int i = 0; i < 999999999; i++)
    {
        int q = sqrt(95426899);
    }
}

void timeloop()
{
    // Continue with your main loop code
    static unsigned long previousTime = 0;

    // Measure the execution time of the loop
    unsigned long currentTime = micros();
    unsigned long elapsedTime = currentTime - previousTime;
    previousTime = currentTime;

    // Print the execution time to the Serial Monitor
    Serial.print("Loop execution time: ");
    Serial.print(elapsedTime);
    Serial.println(" microseconds");
}

void connectToBroker(char *clientID)
{
    Serial.print("Client ID: ");
    Serial.println(clientID);
    if (client.connect(clientID))
    {
        Serial.println("Connected to MQTT broker");
        client.subscribe("topic");
    }
    else
    {
        Serial.print("Failed to connect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println("Retrying in 5 seconds...");
        delay(5000);
    }
}

void mqttBaseSetup()
{
    mqttPort = 1883;

    // Create and initialize a hardware timer
    timer = timerBegin(0, 80, true);              // Timer 0, prescaler 80 (1MHz tick), count up
    timerAttachInterrupt(timer, &timerISR, true); // Attach the ISR function
    timerAlarmWrite(timer, 100000000, true);      // Set the timer interval to 1 second (1,000,000 microseconds)
    timerAlarmEnable(timer);                      // Enable the timer alarm

    // // Connect to Wi-Fi
    WiFi.mode(WIFI_STA); // Optional

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to Wi-Fi...");
    }
    Serial.printf("Connected to Wi-Fi: %s, port %d \n", WiFi.localIP().toString(), mqttPort);

    // Connect to MQTT broker
    client.setServer(mqttBroker, mqttPort);

    // client.setCallback(callback);
    // while (!client.connected())
    // {
    //     connectToBroker();
    // }
}