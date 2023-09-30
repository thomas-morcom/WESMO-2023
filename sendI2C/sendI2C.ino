#include <Wire.h>

// const int address = 8;  // Address of the receiver ESP32

// void setup() {
//   Wire.begin();        // Initialize I2C communication
//   Serial.begin(115200); // Initialize serial communication
// }

// void loop() {
//   int dataToSend = 123; // Integer to send
//   Wire.beginTransmission(address); // Begin transmission to the receiver
//   Wire.write((byte*)&dataToSend, sizeof(dataToSend)); // Write the integer to the I2C bus
//   Wire.endTransmission(); // End the transmission

//   Serial.println("Integer sent: " + String(dataToSend));
//   delay(1000); // Delay before sending again
// }
 
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}
 
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}
