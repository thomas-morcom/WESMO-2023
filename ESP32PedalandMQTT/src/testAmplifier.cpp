#include <Arduino.h>
#include <stdio.h>

// the number of the LED pin
const int ledPin = 16; // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

void setupChannelRead()
{
    // configure LED PWM functionalitites
    ledcSetup(ledChannel, freq, resolution);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(ledPin, ledChannel);

    // pinMode(LED_BUILTIN, OUTPUT);
    // ledcAttachPin(LED_BUILTIN, ledChannel);
}

void loopSendPWM()
{
    // Serial.println("In loop");
    // increase the LED brightness
    for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)
    {
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        Serial.println(dutyCycle);
        delay(15);
    }

    // decrease the LED brightness
    for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--)
    {
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        Serial.println(dutyCycle);
        delay(15);
    }
}