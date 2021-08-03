//GPAC: Spanish Aerocar Control
//By Palmer Insull palmeri@little-canada.ca

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 9);

int distance_pulses = 20000;
int homing_stop = 10;
int waiting_time;

void setup() {
  stepper.setMaxSpeed(700);              // Slower Speed of stepper01 motor for homing
  stepper.setSpeed(700);
  stepper.setAcceleration(200);
  stepper.setMinPulseWidth(50);
  Serial.begin(9600);

  pinMode(1,INPUT);
  pinMode(7,OUTPUT);

}
void loop() {

  if (digitalRead(1) == HIGH) {
    stepper.moveTo(stepper.currentPosition() - 20);
  }

  stepper.run();



  if (digitalRead(1) == LOW) {
    delay(waiting_time);
    digitalWrite(7,LOW);
    stepper.setCurrentPosition(0);
    stepper.runToNewPosition(distance_pulses);
    digitalWrite(7,HIGH);
    delay(waiting_time);
    digitalWrite(7,LOW);
    stepper.runToNewPosition(homing_stop);
    digitalWrite(7,HIGH);
  }
  
}
