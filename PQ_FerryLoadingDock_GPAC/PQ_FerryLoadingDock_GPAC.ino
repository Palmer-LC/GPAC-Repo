//Ferry Loading Dock (Petit Quebec)
//Backyard Rink & Skaters (Petit Quebec) 

//Modifed from: 
//GPAC General Code
//By: Palmer Insull, palmeri@little-canada.ca
//Started: 7/6/2021

//Libraries

//Custom SAMD Servo Library
#include <Servo.h>

//Accel Stepper Library
#include <AccelStepper.h>

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

//Hall sensor and stuff related to the stop/start of ferry loading 
int hallSensor = 0; 
int hallSwitchCounter = 0;  // counter for the number of button presses
int hallState = 0;          // current state of the button
int lastHallState = 0;      // previous state of the button
int timeThreshold = 3500;   // how long between magnet detections 
int numMagnets = 6;         // number of magnets in the magnorail in succession that'll trigger the sensor
int timeDelay = 6000;       // once the cars are loaded, delay by 6 seconds before starting motor again
bool hallFlag = false; 
unsigned long previousTime = millis();
unsigned long currentTime = millis(); 
unsigned long pauseTime = 0; 

//PWM resolution (Default of 255)
float PWM_resolution = 255;

///////////////////
//MOTION PROFILES//
///////////////////

//For some reason everything fails when running it as a constant speed so this looks more complicated than it actually is 
//To review later if necessary... 

//Profile 1 (Speed Profile) //Backyard Rink 
String Profile_1_Stages[5] = {"A", "C", "A"};
float Profile_1_Setpoints[5] = {0.35, 0.35, 0.35};
float Profile_1_Timings[5] = {1, 1, 1};

//Profile 2 (Speed Profile) //Ferry Loading Dock
String Profile_2_Stages[5] = {"A", "C", "A"};
float Profile_2_Setpoints[5] = {0.9, 0.9, 0.9};
float Profile_2_Timings[5] = {1, 15, 1};

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
};

int apple = 0;

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

JOURNEY_CONFIGURATION Journey_1 {"Motor_1_Direction", no_servo, APIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_1_Stages, Profile_1_Setpoints, Profile_1_Timings, true};
JOURNEY_CONFIGURATION Journey_2 {"Motor_1_Direction", no_servo, BPIN1, 0, 0, 0, 12, 0, 0, 0, 0, Profile_2_Stages, Profile_2_Setpoints, Profile_2_Timings, true};

const int Num_Journeys = 2;
JOURNEY_CONFIGURATION Journeys[Num_Journeys] = {Journey_1, Journey_2};

void setup() {

  //Initializing Input Pins
  pinMode(OPTO_IN, INPUT);
  pinMode(IN_1, INPUT); 

  //Serial Monitor
  Serial.begin(9600);

  /////////////////
  //MOTOR CONTROL//
  /////////////////

  //Initialize Pins For Drivers, A, B, C, D

  //Initializing PWM Pins
  pinMode(APIN1, OUTPUT);
  pinMode(APIN2, OUTPUT);

  pinMode(BPIN1, OUTPUT);
  pinMode(BPIN2, OUTPUT);

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
    if (Journey.Device == "Servo"){
      Journey.servo.attach(Journey.PIN1);
    }
  }

  //Holding in idle. 
  //Startup();

  //Reseting start time.
  Reset_Start_Time();
 
}

//Resting Start Times (On Startup)
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
  Journeys[0].ACTIVE = true; 
  hallState = digitalRead(IN_1); 
  
  if (hallState != lastHallState) {
    // if the state has changed, increment the counter    
    if (hallState == LOW) {
      // if the current state is HIGH then the button went from off to on:
      currentTime = millis();
      if (currentTime - previousTime < timeThreshold){
        hallSwitchCounter++;
      }
      else if (currentTime - previousTime >= timeThreshold){
        hallSwitchCounter = 1; 
      }
      previousTime = currentTime;
      
      Serial.print("number of magnets passed: ");
      Serial.println(hallSwitchCounter);
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("hall off");
    }
    // Delay a little bit to avoid bouncing
     delay(50); 
    if (hallSwitchCounter % numMagnets == 0) {
      Serial.println("STOP!");
      hallSwitchCounter = 1;
      Journeys[1].ACTIVE = false; 
      hallFlag = true; 
      pauseTime = millis();   
    }
  }
  lastHallState = hallState;

  if (hallFlag) {
      long currTime = millis(); 
      Journeys[1].Speed = 0; 
      motor_run_1_direction(Journeys[1]);
      if (currTime - pauseTime > timeDelay){
        Serial.println("active again");
        Journeys[1].ACTIVE = true; 
        hallFlag = false; 
      }
    }

  for (int J = 0; J < Num_Journeys; J++) {
    JOURNEY_CONFIGURATION &Journey = Journeys[J];
    if (J == 0){
      Serial.println(Journeys[J].ACTIVE); 
    }
    if (Journey.ACTIVE) { 
      long TIME = millis();
      int zone = Journey.Zone;
      long elapsed_time = TIME - Journey.Start_Time;

      if (Journey.Stages[zone] != "C") {
        Journey.Speed = float(Journey.Last_Speed) + float(elapsed_time) * float(Journey.Profile_Setpoints[zone] - Journey.Last_Speed) / float(1000 * Journey.Profile_Timings[zone]);
      }

      if (elapsed_time >= (Journey.Profile_Timings[Journey.Zone] * 1000)) {
        next_stage(Journey);
      }

      if (Journey.Device == "Servo") {
        servo_run(Journey);
      } else if (Journey.Device == "Motor_1_Direction") {
        motor_run_1_direction(Journey);
      }
    }
  }
}


void next_stage(JOURNEY_CONFIGURATION &Journey) {

  //Incrementing Zone
  Journey.Zone++;

  //Restarting
  if (Journey.Zone == 3) {
    Journey.ACTIVE = false;
    Journey.Zone = 0;
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

void servo_run(JOURNEY_CONFIGURATION &Journey) {
  Journey.servo.write(Journey.Speed);
}
