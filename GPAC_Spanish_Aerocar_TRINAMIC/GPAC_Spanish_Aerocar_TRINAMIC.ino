//GPAC: Spanish Aerocar Control
//By Palmer Insull palmeri@little-canada.ca

#include <AccelStepper.h>

boolean homing = true;

AccelStepper stepper(AccelStepper::DRIVER, 3, 9);

void setup() {
  stepper.setMaxSpeed(700);              // Slower Speed of stepper01 motor for homing
  stepper.setSpeed(700);
  stepper.setAcceleration(200);
  stepper.setMinPulseWidth(50);
  Serial.begin(9700);

  pinMode(1,INPUT);
  pinMode(7,OUTPUT);
}
void loop() {

  if (digitalRead(1) == HIGH and homing) {
    stepper.moveTo(stepper.currentPosition() - 2000);
    homing = false;
  }
    
  if (!homing and stepper.distanceToGo() == 0){
    digitalWrite(7,HIGH);
    while (true){}
  }

  stepper.run();


  if (digitalRead(1) == LOW) {
    digitalWrite(7,HIGH);
    delay(3000);
    digitalWrite(7,LOW);
    stepper.setCurrentPosition(0);
    stepper.runToNewPosition(42000);
    digitalWrite(7,HIGH);
    delay(3000);
    digitalWrite(7,LOW);
    stepper.runToNewPosition(1000);
    homing = true;
  }
  
}
