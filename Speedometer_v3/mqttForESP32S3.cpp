#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "notwifi";
const char *password = "123454321";
const char *mqtt_server = "52.64.104.47";
const int mqtt_port = 1883;
const char *mqtt_topic = "newTopic";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) {
  // Convert the payload to a string
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Check if the message starts with "06000009"
  if (message.startsWith("06000009")) {
    // Do something with the message
    Serial.println("Received message starting with '06000009': " + message);
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

void mqttSetup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void mqttLoop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
