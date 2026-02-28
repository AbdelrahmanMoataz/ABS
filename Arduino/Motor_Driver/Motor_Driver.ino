// Motor driver
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10
#define EN1 6 // PWM
#define EN2 11 // PWM

// Motor PWM speed values
#define MAX 255
#define MIN 0 

enum direction {
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};


void setup() {
  // put your setup code here, to run once:
    // Motor driver pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(EN1, OUTPUT);
    pinMode(EN2, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
 carControl(FORWARD, 78);
}

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