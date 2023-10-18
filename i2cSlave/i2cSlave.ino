#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>

#define SDA_PIN 17
#define SCL_PIN 18
#define I2C_SLAVE_ADDR 0x04

#define LED 2

void receiveEvent(int howMany);

void setup()
{
    Serial.begin(115200);
    // delay(1000);
    Serial.println("--- SETUP ---");
    // pinMode(LED, OUTPUT);
    bool success = WireSlave.begin(SDA_PIN, SCL_PIN, I2C_SLAVE_ADDR);
    if (!success) {
        Serial.println("I2C slave init failed");
        while(1) delay(100);
    }

    WireSlave.onReceive(receiveEvent);
}

void loop()
{
    // the slave response time is directly related to how often
    // this update() method is called, so avoid using long delays
    // inside loop(), and be careful with time-consuming tasks
    WireSlave.update();

    // let I2C and other ESP32 peripherals interrupts work
    delay(1);
}

// function that executes whenever a complete and valid packet
// is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  Serial.println("Received Event");
    while (1 < WireSlave.available()) // loop through all but the last byte
    {
        char c = WireSlave.read();  // receive byte as a character
        Serial.print(c);            // print the character
    }

    int x = WireSlave.read();   // receive byte as an integer
    Serial.println(x);          // print the integer

    if(x == 1)
    {
        Serial.println("Setting LED active HIGH ");
        // digitalWrite(LED, HIGH);
    }
    else if(x == 0)
    {
        Serial.println("Setting LED active LOW ");
        // digitalWrite(LED, LOW);
    }
     Serial.println("");

}




// #include <Wire.h>

// #define I2C_SLAVE_ADDR 0x12 // Replace with your desired address

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Setup");
//   Wire.begin(I2C_SLAVE_ADDR);
//   Wire.onRequest(requestEvent); // Register the requestEvent function
// }

// void loop() {
//   delay(100);
// }

void requestEvent() {
  Wire.write("hello "); // Respond with a message of 6 bytes as expected by the master
}


