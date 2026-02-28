#include <Servo.h>


// #################################################################### OBJECTS AND DEFINITIONS ####################################################################
// ######### OUTPUT PINS #########
// Servos
#define LEFT_BRAKE 4
#define RIGHT_BRAKE 5
// Braking mode indication LEDs
#define RGB_MODE_BLUE 12
#define RGB_MODE_GREEN 13
// Motor driver
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10
#define EN1 6 // PWM
#define EN2 11 // PWM

// ######### INPUT PINs #########
// Bluetooth pairing state
#define STATE A2
// Wheel speed sensors (digital)
#define LEFT_WHEEL 2
#define RIGHT_WHEEL 3
#define BACK_WHEELS A1

// ######### NUMBERS #########
// Servo angles
#define OPEN 0
#define CLOSED 180
#define LEFT_OPEN 100
#define LEFT_CLOSED 0
#define RIGHT_OPEN 100
#define RIGHT_CLOSED 0


// Motor PWM speed values
#define MAX 255
#define MIN 0

// Brake mode
#define MANUAL 1
#define ABS 0

// Minimum wheel speed at which the car is considered stopped
#define MIN_WHEEL_SPEED 0.01

// Speed calculation period (in ms)
#define PERIOD 100.0 // (changed to add decimal point)

// Constants
#define PI 3.1415926535
#define WHEEL_DIAMETER 65.0 // in mm (changed to add decimal point)
#define NUMBER_OF_SLITS 20.0 // number of slits of encoder wheel (because we interrupt on change, we double the number of slits)
#define WHEEL_SENSOR_NUM 3

// ######### GLOBAL OBJECTS AND VARIABLES #########
// Servo objects
Servo leftBrake;
Servo rightBrake;

// Time variables
unsigned long current;
unsigned long last = 0;

// Interrupt variables
volatile unsigned int left_wheel_steps = 0;
volatile unsigned int right_wheel_steps = 0;
volatile unsigned int back_wheels_steps = 0;

// Calculation variables
double slip_ratio = 0;
double vehicle_velocity = 0;
double left_wheel_velocity = 0;
double right_wheel_velocity = 0;

// Bluetooth data variable
String command = "";
String brake_command = "";
int brake_mode = MANUAL;




enum direction {
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

// #################################################################### SETUP AND LOOP ########################################################################
void setup() {
    // Servo pins
    leftBrake.attach(LEFT_BRAKE);
    rightBrake.attach(RIGHT_BRAKE);
    pinMode(LEFT_BRAKE, OUTPUT);
    pinMode(RIGHT_BRAKE, OUTPUT);

    // Braking mode LED pins
    pinMode(RGB_MODE_BLUE, OUTPUT);
    pinMode(RGB_MODE_GREEN, OUTPUT);

    // Motor driver pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(EN1, OUTPUT);
    pinMode(EN2, OUTPUT);

    // Speed sensors pins (pullup for interrupts)
    pinMode(LEFT_WHEEL, INPUT_PULLUP);
    pinMode(RIGHT_WHEEL, INPUT_PULLUP);
    pinMode(BACK_WHEELS, INPUT_PULLUP);

    // Bluetooth pairing state pin
    pinMode(STATE, INPUT);

    // Bluetooth
    Serial.begin(9600);  // Default communication rate of the Bluetooth module

    // Interrupts
    // Hardware interrupts
    attachInterrupt(digitalPinToInterrupt(LEFT_WHEEL), leftIncrement, CHANGE);
    attachInterrupt(digitalPinToInterrupt(RIGHT_WHEEL), rightIncrement, CHANGE);
    // Pin change interrupt
    PCICR |= B00000010; // Pin Change Interrupt Control Register, setting bit 1 means we enabled port C interrupt
    PCMSK1 |= B00000010; // Pin Change Mask 1 -> We are enabling pin A1 to activate the PORT C ISR

    // Initialise system then wait for bluetooth connection
    systemReset();
    //while (!connectionEstablished());
}


void loop() {
    
    if (Serial.available() > 0) {  // Checks whether data is coming from the serial port
        command = Serial.readStringUntil('\n');     // Reads the data from the serial port
    }
    carMode(command);
    //Serial.print(command);
    command = ""; // reset the command, wait until new command given
    // After executing command, check for connection to computer
    // If not connected, reset the system and then wait for reconnection
   // if (!connectionEstablished()) {
        //systemReset();
        //while (!connectionEstablished());
    //}
}

// #################################################################### FUNCTION DEFINITIONS #######################################################################
/*long mapf(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) * 1.0 / (in_max - in_min) * 1.0 + out_min * 1.0;
}*/
// ############ BRAKES FUNCTIONS ###############
// Interrupt ISRs

void leftIncrement(){
    left_wheel_steps++;
}

void rightIncrement(){
    right_wheel_steps++;
}

ISR(PCINT1_vect){
    back_wheels_steps++;
}


// Speed is updated every 20ms, testing will show if this is too slow
void calculateSpeed(int wheel_sensed, double* wheel_velocity){
    // Time variables
    static unsigned long start_time[WHEEL_SENSOR_NUM] = {0};
    static unsigned long end_time[WHEEL_SENSOR_NUM] = {0};
    
    // Step variables
    static unsigned long steps[WHEEL_SENSOR_NUM] = {0};
    static unsigned long steps_old[WHEEL_SENSOR_NUM] = {0};
    unsigned long diff;
    int index;

    // Select which wheels variables will be currently used
    switch(wheel_sensed){
        case LEFT_WHEEL:
            index = 0;
            break;

        case RIGHT_WHEEL:
            index = 1;
            break;

        case BACK_WHEELS:
            index = 2;
            break;
        
        default:
            index = 0;
            break;
    }

    // Calculate wheel speed in mm/ms
    if(end_time[index] - start_time[index] > PERIOD){
        // Steps update, making sure to copy values from interrupt variables so they don't change during calculation
        noInterrupts();
        steps[0] = left_wheel_steps;
        steps[1] = right_wheel_steps;
        steps[2] = back_wheels_steps;
        interrupts();
        // Calculation is: number of revolutions (diff / number of slits) * PI * wheel diameter = distance travelled by wheel in mm
        // Divide by period to get the speed. Note: This only works 
		diff = steps[index] - steps_old[index];
        *wheel_velocity = (diff * PI * WHEEL_DIAMETER / (NUMBER_OF_SLITS * 2)) / (PERIOD); // multiplying number of slits by 2 because steps are read by change of state instead of at HIGH
        start_time[index] = end_time[index];
		steps_old[index] = steps[index];
    }
    end_time[index] = millis();
}

// make the 20ms delay only occur at state change not everytime function is called
void brakes(int state) {
  if (state) {
    leftBrake.write(LEFT_CLOSED);
    rightBrake.write(RIGHT_CLOSED);
    //delay(20);
  } 
  else {
    leftBrake.write(LEFT_OPEN);
    rightBrake.write(RIGHT_OPEN);
    //delay(20);
  }
}

void ABS_FUNC() {
  slip_ratio = 1 - (left_wheel_velocity / vehicle_velocity);
  if (slip_ratio < 0.1)
    brakes(CLOSED);
  else if (slip_ratio > 0.3)
    brakes(OPEN);
}

// Try to only call the digitalWrite functions once during mode change
void brakeControl() {
  switch (brake_mode) {
    case MANUAL:  // Manual mode. Engaged when brake button is pressed, disengages when brake button is released
      if (brake_command == "B")
        brakes(CLOSED);
      else
        brakes(OPEN);
      break;

    case ABS:  // ABS mode. When brake is pressed, uses ABS to prevent the wheel from locking during travel
      if (brake_command == "B")
        ABS_FUNC();
      else
        brakes(OPEN);

      break;

    default:
      break;
  }
}

// ############ CAR FUNCTIONS ###############
// Not sure if LEFT and RIGHT will actually go left or right, need to test
void carControl(int direction, unsigned int speed) {
  switch (direction) {
    case STOP:
      analogWrite(EN1, speed);
      analogWrite(EN2, speed);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      break;

    case FORWARD:
      analogWrite(EN1, speed);
      analogWrite(EN2, speed);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      break;

    case BACKWARD:
      analogWrite(EN1, speed);
      analogWrite(EN2, speed);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      break;

    case LEFT:
      analogWrite(EN1, speed);
      analogWrite(EN2, speed);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      break;

    case RIGHT:
      analogWrite(EN1, speed);
      analogWrite(EN2, speed);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      break;

    default:
      break;
  }
}

void carMode(String mode) {
    // Start: The car moves forward at maximum speed
    if(mode == "S"){
        carControl(FORWARD, MAX);
    }

    // Stop: The car instantly stops, motor acts as brakes
    if(mode == "s"){
      carControl(STOP, 0);
    }

    // Manual: Change the braking mode to manual
    if(mode == "M"){
        brake_mode = MANUAL;
        digitalWrite(RGB_MODE_GREEN, HIGH);
        digitalWrite(RGB_MODE_BLUE, LOW);
    }

    
    // ABS: Change the braking mode to ABS
    if(mode == "A"){
        brake_mode = ABS;
        digitalWrite(RGB_MODE_BLUE, HIGH);
        digitalWrite(RGB_MODE_GREEN, LOW);
    }

    // Brakes: Starts the braking process
    if(mode == "B"){
        int speed = 255;
        // Enter braking loop, ends when car comes to a halt
        while (speed >= 0) {
            // Receive commands during braking process
            if (Serial.available() > 0)  // Checks whether data is coming from the serial port
                brake_command = Serial.readStringUntil('\n');   // Reads the data from the serial port
            brakeControl(); // Start braking
            // Calculate speed for each wheel
            calculateSpeed(LEFT_WHEEL, &left_wheel_velocity);
            calculateSpeed(BACK_WHEELS, &vehicle_velocity);
            // Decrease the speed of the backwheels every set interval
            current = millis();
            if (current - last > 5000/255) { // this if condition ensures motors stop rotating after 5 seconds
                carControl(FORWARD, speed--); // change speed and decrement at same time
                last = current;
            }
            // Stops the vehicle early at low speeds OR when it is commanded to stop
            if (brake_command == "s")
                break;
        }
        brake_command = "";
        brakes(OPEN);
        carControl(STOP, 0); // after braking loop ends, make sure to send LOW signal to all motor drive pins
    }
}

// ############ SYSTEM FUNCTIONS ###############
void systemReset() {
  // Initialisation of brakes, will be open
  brakes(OPEN);

  // Initialisation of RGB LED, will be turned off
  digitalWrite(RGB_MODE_BLUE, HIGH);
  digitalWrite(RGB_MODE_GREEN, HIGH);

  // Initialisation of motor driver, all turned off
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
}

int connectionEstablished() {
  return digitalRead(STATE);
}
