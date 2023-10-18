#include <Arduino.h>
#include <stdio.h>

// put function declarations here:
// int myFunction(int, int);

void setupLED()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello from setup");
}

void loopLED()
{
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Hello from loop");
}

// put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }