#include <Wire.h>

// void setup()
// {
//   Serial.begin(9600);           // start serial for output
//   Serial.println("Setup");
//   Wire.begin(4);                // join i2c bus with address #4
//   Wire.onReceive(receiveEvent); // register event
// }

// void loop()
// {
//   delay(100);
//   // Serial.println("Loop");
// }

// // function that executes whenever data is received from master
// // this function is registered as an event, see setup()
// void receiveEvent(int howMany)
// {
//   Serial.println("received event");
//   while(1 < Wire.available()) // loop through all but the last
//   {
//     char c = Wire.read(); // receive byte as a character
//     Serial.print(c);         // print the character
//   }
//   int x = Wire.read();    // receive byte as an integer
//   Serial.println(x);         // print the integer
// }

// Include the required Wire library for I2C<br>#include <Wire.h>
// int LED = 13;
int x = 0;
void setup() {
  Serial.begin(9600);           // start serial for output
  Serial.println("Setup"); 

  // Define the LED pin as Output
  // pinMode (LED, OUTPUT);
  // Start the I2C Bus as Slave on address 9
  Wire.begin(8); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
}
void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
  Serial.print("X is ");
  Serial.println(x);
}
void loop() {
  //If value received is 0 blink LED for 200 ms
  // if (x == '0') {
  //   // digitalWrite(LED, HIGH);
  //   delay(200);
  //   // digitalWrite(LED, LOW);
  //   delay(200);
  // }
  // //If value received is 3 blink LED for 400 ms
  // if (x == '3') {
  //   // digitalWrite(LED, HIGH);
  //   delay(400);
  //   // digitalWrite(LED, LOW);
  //   delay(400);
  // }

  Serial.println("in da loop");
  delay(1000);
}
