#include <Servo.h>


// #################################################################### OBJECTS AND DEFINITIONS ####################################################################
Servo leftBrake;  // create servo object to control a servo
Servo rightBrake;  // create servo object to control a servo

int brake_mode;
int brake_L;
int brake_R;

int brake_button;

// OUTPUTS
// Servos
#define LEFT_BRAKE 
#define RIGHT_BRAKE
// Braking mode indication LEDs
#define RGB_MODE_RED
#define RGB_MODE_GREEN
// Motor driver
#define IN1
#define IN2
#define IN3
#define IN4
#define EN1   // PWM
#define EN2   // PWM

// INPUTS
// Bluetooth pairing state
#define STATE
// Wheel speed sensors (digital)
#define LEFT_WHEEL
#define RIGHT_WHEEL


#define OPEN 0
#define CLOSED 180
#define MAX 255
#define MIN 0
enum direction{
  FORWARD = 1,
  BACKWARD,
  LEFT,
  RIGHT
}

// #################################################################### SETUP AND LOOP ########################################################################
int pos1 = 0;    // variable to store the servo position
int pos2 = 0;
void setup() {
  // Servo pins
  leftBrake.attach(LEFT_BRAKE);  
  rightBrake.attach(RIGHT_BRAKE);
  pinMode(LEFT_BRAKE, OUTPUT);
  pinMode(RIGHT_BRAKE, OUTPUT);

  // Braking mode LED pins
  pinMode(RGB_MODE_RED, OUTPUT);
  pinMode(RGB_MODE_GREEN, OUTPUT);

  // Motor driver pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  // Bluetooth pairing state pin
  pinMode(STATE, INPUT);

  // Speed sensors pins
  pinMode(LEFT_WHEEL, INPUT);
  pinMode(RIGHT_WHEEL, INPUT);

  //Bluetooth
  Serial.begin(38400); // Default communication rate of the Bluetooth module
}

void loop{

}





int state = 0;

void loop() {
  if(Serial.available() > 0){ // Checks whether data is coming from the serial port
    state = Serial.read(); // Reads the data from the serial port
  }

  if (state == '0') {
      digitalWrite(ledPin, LOW); // Turn LED OFF
      Serial.println("LED: OFF"); // Send back, to the phone, the String "LED: ON"
      state = 0;
  }
    else if (state == '1') {
        digitalWrite(ledPin, HIGH);
        Serial.println("LED: ON");;
        state = 0;
    } 
  }

// #################################################################### FUNCTION DEFINITIONS #######################################################################
// Not sure if LEFT and RIGHT will actually go left or right, need to test
void carControl(int direction, unsigned int speed){
  switch(direction){
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
  }
}

void carMode(char mode){
  switch(mode){
    case 'F':
    carControl("FORWARD", MAX);
    break;

    case 'B':
    for(int i = MAX; i > MIN; i--){
      carControl("FORWARD", i);
      delay(100); // change this to use millis, use map() (or mapf you created) to convert 0 - 255 range to time in seconds
    }
    break;

    default:
    break;
  }
}


void ABS(){
  hmm

}

// make the 20ms delay only occur at state change not everytime function is called
void brakeControl(char mode){
  switch(mode){
    case 'M': // Manual mode. Engaged when brake is pressed, disengages when brake button released
      if(brake_button){
        leftBrake.write(CLOSED);
        rightBrake.write(CLOSED);
        delay(20);
      }
      else{
        leftBrake.write(OPEN);
        rightBrake.write(OPEN);
        delay(20);
      }
      break;

    case 'A': // ABS mode. When brake is pressed, uses ABS to prevent the wheel from locking during travel
      ABS();
      break;

    default:  // At default brakes are disengaged
      leftBrake.write(OPEN);
      rightBrake.write(OPEN);
      delay(20);
      break;
  }
}
