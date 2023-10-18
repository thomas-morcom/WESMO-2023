#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>

const char *ssid = "mywifi";
const char *password = "1234567890";
const char *mqtt_server = "52.64.104.47";
const int mqtt_port = 1883;
const char *mqtt_topic = "newTopic";

int speed = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// void callback(char *topic, byte *payload, unsigned int length) {
//   // Convert the payload to a string
//   String message = "";
//   for (unsigned int i = 0; i < length; i++) {
//     message += (char)payload[i];
//   }

//   // Check if the message starts with "06000009"
//   if (message.startsWith("06000009")) {
//     // Do something with the message
//     Serial.println("Received message starting with '06000009': " + message);
//   }
// }

int getSpeed() {
  return speed;
}

void splitString(const String &input, char delimiter, String parts[], int maxParts) {
  int partIndex = 0;
  int startIndex = 0;
  int length = input.length();

  for (int i = 0; i < length; i++) {
    if (input[i] == delimiter) {
      if (partIndex < maxParts) {
        parts[partIndex] = input.substring(startIndex, i);
        partIndex++;
      }
      startIndex = i + 1;
    }
  }

  // Add the last part of the string
  if (partIndex < maxParts) {
    parts[partIndex] = input.substring(startIndex);
  }
}

// Assuming this is inside the callback function
void callback(char *topic, byte *payload, unsigned int length) {
  // Convert the payload to a string
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Check if the message starts with "06000009"
  if (message.startsWith("06000009")) {
    Serial.println("Received message starting with '06000009': " + message);

    // Split the message by spaces
    int partCount = 9;
    String parts[partCount];  // Assuming there are always 9 parts
    splitString(message, ' ', parts, 9);

    // Check if there are at least two parts
    if (partCount >= 2) {
      // Convert the last two parts (representing the number) to integers
      int num1 = strtol(parts[partCount - 2].c_str(), nullptr, 16);
      int num2 = strtol(parts[partCount - 1].c_str(), nullptr, 16);

      // Combine the two numbers
      speed = (num1 << 8) | num2;

      // Do something with the combined number
      Serial.print("Combined number: ");
      Serial.println(speed);
    } else {
      Serial.println("Invalid message format");
    }
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32S3Client")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// void mqttSetup() {
//   // Serial.begin(115200);
//   Serial.println("--SETUP--- MQTT");
//   WiFi.begin(ssid, password);
//   Serial.println("After Wifi.begin()");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }

//   client.setServer(mqtt_server, mqtt_port);
//   client.setCallback(callback);
//   Serial.println("--END OF SETUP--- MQTT");
// }
void mqttSetup() {
  Serial.println("--SETUP--- MQTT");
  Serial.println("--END OF SETUP--- MQTT");
}

void mqttLoop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
