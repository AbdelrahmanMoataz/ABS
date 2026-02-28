int x = 95;
double num1 = 1.12321;
double num2 = 524.1244;
#include <Servo.h>
// Servos
#define LEFT_BRAKE 4
#define RIGHT_BRAKE 5

// Servo angles
#define OPEN 0
#define CLOSED 180

// Brake mode
#define MANUAL 1
#define ABS 0

// Minimum wheel speed at which the car is considered stopped
#define MIN_WHEEL_SPEED 5

// Constants
#define PI 3.1415926535
#define WHEEL_DIAMETER 65 // in mm
#define NUMBER_OF_SLITS 20 // number of slits of encoder wheel
#define WHEEL_SENSOR_NUM 3

// ######### GLOBAL OBJECTS AND VARIABLES #########
// Servo objects
Servo leftBrake;
Servo rightBrake;

// Time variables
unsigned long current;
unsigned long last = 0;

// Calculation variables
double slip_ratio = 0;
const double vehicle_velocity = 0;
const double wheel_velocity = 0;

enum direction {
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

void setup() {
  // put your setup code here, to run once:
    // Servo pins
    leftBrake.attach(LEFT_BRAKE);
    rightBrake.attach(RIGHT_BRAKE);
    pinMode(LEFT_BRAKE, OUTPUT);
    pinMode(RIGHT_BRAKE, OUTPUT);
    Serial.begin(9600);
    //Serial.println("START: ");
}

void loop() {
  // put your main code here, to run repeatedly:
    /*if(!Serial.available())
      x = Serial.readStringUntil('\n');
    if(x == "S")
        brakes(CLOSED);

    if(x == "s")
        brakes(OPEN);*/

    /*
    brakes(OPEN);
    delay(1000);
    brakes(CLOSED);
    delay(1000);*/
    
    for(int i = 0; i <= 180; i++){
        leftBrake.write(i);
        Serial.println(i);
        delay(200);
    }
    /*
    if (Serial.available() > 0){
        x = Serial.readStringUntil('\n').toInt();
        Serial.println(x);
    }

    rightBrake.write(x);*/
    /*
    if (millis() - last > 5000/255) { // this if condition ensures motors stop rotating after 5 seconds
        Serial.print(num1);
        Serial.print(".");
        Serial.println(num2);
        last = millis();
    }*/
}

//############ BRAKES FUNCTIONS ###############


// make the 20ms delay only occur at state change not everytime function is called
void brakes(int state) {
  if (state) {
    leftBrake.write(0);
    rightBrake.write(0);
    //delay(20);
  } 
  else {
    leftBrake.write(100);
    rightBrake.write(100);
    //delay(20);
  }
}

