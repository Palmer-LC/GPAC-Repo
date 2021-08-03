/**
 * Author Teemu Mäntykallio
 * Initializes the library and runs the stepper motor.
 */


#include <AccelStepper.h>

void setup() {
   
    pinMode(3,OUTPUT);
}

void loop() {
    digitalWrite(3,HIGH);
    delayMicroseconds(100);
     digitalWrite(3,LOW);
    delayMicroseconds(100);
}
