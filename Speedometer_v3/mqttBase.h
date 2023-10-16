#ifndef MQTTBASE_H
#define MQTTBASE_H

extern const char *ssid;
extern const char *password;
extern const char *mqttBroker;
extern const int mqttPort;
// extern const char *mqttClientID;

extern hw_timer_t *timer;

extern PubSubClient client;

void mqttBaseSetup();

void timerISR();

void timeloop();

void connectToBroker(char *clientID);

#endif