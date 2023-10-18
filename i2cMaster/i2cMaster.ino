#include <Arduino.h>
#include <Wire.h>
#include <WirePacker.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04

void setup()
{
    Serial.begin(115200);           // start serial for output
    Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
}

void loop()
{
    static unsigned long lastWireTransmit = 0;
    static byte x = 0;

    // send data to WireSlave device every 1000 ms
    if (millis() - lastWireTransmit > 1000) {
        // first create a WirePacker that will assemble a packet
        WirePacker packer;

        // then add data the same way as you would with Wire
        packer.write("x is ");
        packer.write(x);

        // after adding all data you want to send, close the packet
        packer.end();

        // now transmit the packed data
        Wire.beginTransmission(I2C_SLAVE_ADDR);
        while (packer.available()) {    // write every packet byte
            Wire.write(packer.read());
        }
        Wire.endTransmission();         // stop transmitting
        lastWireTransmit = millis();
        if(x==0)
          x = 1;
        else
        x = 0;
    }
}