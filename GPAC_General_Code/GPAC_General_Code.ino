//GPAC General Code
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

//Profile 1 (Speed Profile) //Horse and Buggy
String Profile_1_Stages[5] = {"A", "C", "A"};
float Profile_1_Setpoints[5] = {0.4, 0.4, 0};
float Profile_1_Timings[5] = {3, 25, 2};

//Profile 2 (Speed Profile) //Farm Train
String Profile_2_Stages[5] = {"A", "C", "A"};
float Profile_2_Setpoints[5] = {0.45, 0.45, 0};
float Profile_2_Timings[5] = {6, 25, 5};

//Profile 3 (Angle Profile) //Forklift
String Profile_3_Stages[3] = {"A", "C", "A","C"};
float Profile_3_Setpoints[3] = {90, 90, 0, 0};
float Profile_3_Timings[3] = {3, 2, 3, 2};

//Profile 4 (Angle Profile) //Grape Stompers
String Profile_4_Stages[3] = {"A", "C", "A"};
float Profile_4_Setpoints[3] = {25, 25, 0};
float Profile_4_Timings[3] = {1, 1, 1};


//Profile 5 (Speed Profile) //Vineyard Fan
String Profile_5_Stages[1] = {"C"};
float Profile_5_Setpoints[1] = {0.1};
float Profile_5_Timings[1] = {0};

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

JOURNEY_CONFIGURATION Journey_1 {"Motor_1_Direction", no_servo, APIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_1_Stages, Profile_1_Setpoints, Profile_1_Timings, true,3}; //Horse and Buggy
JOURNEY_CONFIGURATION Journey_2 {"Motor_1_Direction", no_servo, BPIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_2_Stages, Profile_2_Setpoints, Profile_2_Timings, true,3}; //Farm Train
JOURNEY_CONFIGURATION Journey_3 {"Servo", servo_1, DPIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_3_Stages, Profile_3_Setpoints, Profile_3_Timings, true,4}; //Forklift
JOURNEY_CONFIGURATION Journey_4 {"Servo", servo_2, DPIN2, 0, 0, 0, 12, 0, 0, 0, 0, Profile_3_Stages, Profile_4_Setpoints, Profile_4_Timings, true,3}; //Grape Stompers
JOURNEY_CONFIGURATION Journey_5 {"Motor_1_Direction", no_servo, CPIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_5_Stages, Profile_5_Setpoints, Profile_5_Timings, true,1}; //Fan


const int Num_Journeys = 5;
JOURNEY_CONFIGURATION Journeys[Num_Journeys] = {Journey_1, Journey_2, Journey_3,Journey_4, Journey_5};

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
  digitalWrite(APIN2,LOW);

  pinMode(BPIN1, OUTPUT);
  pinMode(BPIN2, OUTPUT);
  digitalWrite(BPIN2,LOW);


  pinMode(CPIN1, OUTPUT);
  pinMode(CPIN2, OUTPUT);
  digitalWrite(CPIN2,LOW);

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

  for (int J = 0; J < Num_Journeys; J++) {
    JOURNEY_CONFIGURATION &Journey = Journeys[J];

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
  }
}

void next_stage(JOURNEY_CONFIGURATION &Journey) {

  //Incrementing Zone
  Journey.Zone++;

  //Restarting
  if (Journey.Zone == Journey.Profile_Length) {
    Journey.Zone = 0;
  }

  //Assigning Stage Properties
  Journey.Last_Speed = Journey.Speed;
  Journey.Start_Time = long(millis());
}

//Runs The 1-Directional Motor
void motor_run_1_direction(JOURNEY_CONFIGURATION &Journey) {

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

    if (Journey.PIN1 == APIN1){
    Serial.println("Pin: " + String(Journey.PIN1) + " Speed: " + String(Journey.Speed) + " Reso: " + String(PWM_resolution*Journey.Speed));
  }
          

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
}
