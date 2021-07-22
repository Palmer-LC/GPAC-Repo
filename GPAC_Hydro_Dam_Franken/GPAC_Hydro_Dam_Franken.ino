//GPAC Hydro Dam: Fraken-Moose Animations (Servo, Lightning, Fans)
//By: Palmer Insull, palmeri@little-canada.ca
//Started: 7/6/2021

//Libraries

//Custom SAMD Servo Library
#include <Servo.h>

//Accel Stepper Library
//#include <AccelStepper.h>

//Pin Definitions For Output Pins
#define APIN1 3
#define APIN2 4

#define BPIN1 5
#define BPIN2 6

#define DPIN1 9
#define DPIN2 10

//Pin Definitions For Input Pins
#define OPTO_IN 0
#define IN_1 1
#define IN_2 2

//Pin Definitions For Selectable Pins
#define CPIN1 7
#define CPIN2 8

//Servo Definitions
Servo servo_1;
Servo servo_2;
Servo servo_3;
Servo servo_4;
Servo servo_5;
Servo servo_6;
Servo servo_7;
Servo servo_8;

//An unused servo to accomdate the Struct initliazition...
Servo no_servo;


////////////////////
//TIME OF DAY CODE//
////////////////////

//Variable to store current time...
long show_time = 0;

//Activation Profiles
long Activation_Profile_1[2] = {5000, 25000};
long Activation_Profile_2[2];
long Activation_Profile_3[2];
long Activation_Profile_4[2];
long Activation_Profile_5[2];
long Activation_Profile_6[2];
long Activation_Profile_7[2];
long Activation_Profile_8[2];

////////////////////////////////////////
//DEFINE SUPPLY AND OPERATING VOLTAGES//
////////////////////////////////////////

//Supply Voltage
float Supply_Voltage = 12;

//////////////////////////
//OTHER GLOBAL VARAIBLES//
//////////////////////////

//PWM resolution (Default of 255)
float PWM_resolution = 255;

///////////////////
//MOTION PROFILES//
///////////////////

//Profile 1 (Speed Profile) //Fans
String Profile_1_Stages[1] = {"C"};
float Profile_1_Setpoints[1] = {0.3};
float Profile_1_Timings[1] = {0};

//Profile 2 (Speed Profile) //Relay For Lightning
String Profile_2_Stages[2] = {"C", "C"};
float Profile_2_Setpoints[2] = {0.5,0};
float Profile_2_Timings[2] = {1,25};

//Profile 3 (Speed Profile) //Servo
String Profile_3_Stages[3] = {"A", "C", "A"};
float Profile_3_Setpoints[3] = {45, 45, 0};
float Profile_3_Timings[3] = {1, 24, 1};

///////////////////////////
//JOURNEY DATA STRUCTURE///
///////////////////////////

struct JOURNEY_CONFIGURATION {
  String Device;
  Servo servo;
  int PIN1;
  int PIN2;
  int PIN3;
  int PIN4;
  float Max_Voltage;
  int Zone;
  float Speed;
  long Start_Time;
  float Last_Speed;
  String *Stages;
  float *Profile_Setpoints;
  float *Profile_Timings;
  boolean ACTIVE;
  boolean CONSTANT;
  int Profile_Length;
};

/////////////////////////////////////
//SETTING UP JOURNEY CONFIGURATIONS//
/////////////////////////////////////

//Maybe make a method to do this more cleanly?

//Device ("Motor_1_Direction", "Motor_2_Direction", "Stepper, "Servo")
//Servo Object (no_servo or servo_#)
//Pin_1, Pin_2, Pin_3, Pin_4
//Max_Voltage,
//Zone (default 0), Speed (default 0), Start_Time (default 0), Last_Speed (default 0)
//Stages[] , Setpoints[], Timings[], Num_Stages, ACTIVE)

JOURNEY_CONFIGURATION Journey_1 {"Motor_1_Direction", no_servo, APIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_1_Stages, Profile_1_Setpoints, Profile_1_Timings, true, true, 1}; //Fans
JOURNEY_CONFIGURATION Journey_2 {"Motor_1_Direction", no_servo, BPIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_1_Stages, Profile_1_Setpoints, Profile_1_Timings, true, true, 1}; //Fans
JOURNEY_CONFIGURATION Journey_3 {"Motor_1_Direction", no_servo, CPIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_2_Stages, Profile_2_Setpoints, Profile_2_Timings, true, true, 2}; //Lightning
JOURNEY_CONFIGURATION Journey_4 {"Servo", servo_1, DPIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_3_Stages, Profile_3_Setpoints, Profile_3_Timings, true, true, 3}; //Servo

const int Num_Journeys = 4;
JOURNEY_CONFIGURATION Journeys[Num_Journeys] = {Journey_1, Journey_2, Journey_3,Journey_4};

void setup() {

  //Initializing Input Pins
  pinMode(OPTO_IN, INPUT);

  //Serial Monitor
  Serial.begin(9600);

  /////////////////
  //MOTOR CONTROL//
  /////////////////

  //Initialize Pins For Drivers, A, B, C, D

  //Initializing PWM Pins
  pinMode(APIN1, OUTPUT);
  pinMode(APIN2, OUTPUT);
  digitalWrite(APIN2, LOW);

  pinMode(BPIN1, OUTPUT);
  pinMode(BPIN2, OUTPUT);
  digitalWrite(BPIN2, LOW);

  pinMode(CPIN1, OUTPUT);
  pinMode(CPIN2, OUTPUT);
  digitalWrite(CPIN2, LOW);  

  //Setting up serial monitor.
  Serial.begin(9600);

  ///////////////////
  //STEPPER CONTROL//
  ///////////////////

  /////////////////
  //SERVO CONTROL//
  /////////////////

  //Attaching Servos
  for (int J = 0; J < Num_Journeys; J++) {
    JOURNEY_CONFIGURATION &Journey = Journeys[J];
    if (Journey.Device == "Servo") {
      Journey.servo.attach(Journey.PIN1);
    }
  }

  //Holding in idle.
  //Startup();

  //Reseting start time.
  Reset_Start_Time();
  
}

//Reseting Start Times (On Startup)
void Reset_Start_Time() {
  //Setting Start Time For All Journeys
  for (int J = 0; J < Num_Journeys; J++) {
    JOURNEY_CONFIGURATION &Journey = Journeys[J];
    Journey.Start_Time = millis();
  }
}

//Startup, Idling
void Startup() {
  while (digitalRead(OPTO_IN) == LOW) {
    //Idling, waiting for synch pulse.
  }
}

/////////////
//MAIN LOOP//
/////////////

void loop() {

  ///////////////////
  //ANIMATION STAGE//
  ///////////////////
/*
  if (!Journeys[0].ACTIVE and !Journeys[1].ACTIVE  and !Journeys[2].ACTIVE  and !Journeys[3].ACTIVE) {
    int rando = random(1, 3);
    JOURNEY_CONFIGURATION &Journey = Journeys[rando];

    Journey.ACTIVE = true;
    Journey.Start_Time = millis();
    Journey.Zone = 0;
  }
  */

  for (int J = 0; J < Num_Journeys; J++) {
    JOURNEY_CONFIGURATION &Journey = Journeys[J];

   // if (Journey.ACTIVE) {

      //Serial.print(String(J) + " ");

      long TIME = millis();
      int zone = Journey.Zone;
      long elapsed_time = TIME - Journey.Start_Time;

      if (Journey.Stages[zone] != "C") {
        Journey.Speed = float(Journey.Last_Speed) + float(elapsed_time) * float(Journey.Profile_Setpoints[zone] - Journey.Last_Speed) / float(1000 * Journey.Profile_Timings[zone]);
      } else {
        Journey.Speed = Journey.Profile_Setpoints[zone];
      }

      if (elapsed_time >= (Journey.Profile_Timings[zone] * 1000)) {
        next_stage(Journey);
      }

      if (Journey.Device == "Servo") {
        servo_run(Journey);
      } else if (Journey.Device == "Motor_1_Direction") {
        motor_run_1_direction(Journey);
      }
    //}
  }
  //Serial.println();
}

void next_stage(JOURNEY_CONFIGURATION &Journey) {

  //Incrementing Zone
  Journey.Zone++;

  //Restarting
  if (Journey.Zone == Journey.Profile_Length) {
    Journey.Zone = 0;
    if (!Journey.CONSTANT) {
      Journey.ACTIVE = false;
    }
  }

  //Assigning Stage Properties
  Journey.Last_Speed = Journey.Speed;
  Journey.Start_Time = long(millis());
}

//Runs The 1-Directional Motor
void motor_run_1_direction(JOURNEY_CONFIGURATION &Journey) {

  if (Journey.Speed < 0 or Journey.Speed > 1) {
    //Serial.println("Invalid: Speed");
    return;
  }

  if (Journey.Max_Voltage > 12) {
    Serial.println("Invalid: Max Voltage Is Above Supply Voltage!");
    return;
  }

  float voltage_limit = Journey.Max_Voltage / 12;

  float limited_speed_ratio = voltage_limit * Journey.Speed;

  analogWrite(Journey.PIN1, limited_speed_ratio * PWM_resolution);

}

//Runs The 2-Directional Motor
void motor_run_2_direction(JOURNEY_CONFIGURATION &Journey) {

  if (Journey.Speed < 0 or Journey.Speed > 1) {
    Serial.println("Invalid: Speed");
    return;
  }

  if (Journey.Max_Voltage > 12) {
    Serial.println("Invalid: Max Voltage Is Above Supply Voltage!");
    return;
  }

  float voltage_limit = Journey.Max_Voltage / 12;

  float limited_speed_ratio = voltage_limit * Journey.Speed;

  analogWrite(Journey.PIN1, limited_speed_ratio * PWM_resolution);

}

void servo_run(JOURNEY_CONFIGURATION &Journey) {
  Journey.servo.write(Journey.Speed);
  Serial.println("HELP");
}
