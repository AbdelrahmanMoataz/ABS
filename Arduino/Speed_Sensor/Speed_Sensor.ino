int sensor = 12;
unsigned long start_time = 0;
unsigned long end_time = 0;
int steps=0;
float steps_old=0;
float temp=0;
float rps=0;

unsigned long last = 0;
enum direction {
  STOP,
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

// Wheel speed sensors (digital)
#define LEFT_WHEEL 12
#define RIGHT_WHEEL 13
#define BACK_WHEELS A0
// Motor driver
#define IN1 5
#define IN2 6
#define IN3 7
#define IN4 8
#define EN1 9 // PWM
#define EN2 10 // PWM
// Constants
#define PI 3.1415926535
#define WHEEL_DIAMETER 65 // in mm
#define NUMBER_OF_SLITS 20 // number of slits of encoder wheel
#define WHEEL_SENSOR_NUM 3
#define PERIOD 1000

void setup() 
{
    // Motor driver pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  Serial.begin(9600);
  pinMode(sensor,INPUT);
  Serial.println("START:");
}
 
void loop()
{
    carControl(FORWARD,78);
    calculateSpeed(12, &rps1);
    //calculateSpeed(13, &rps2);
    
    if(millis() - last > 1000){
        Serial.print("RPS 1: ");
        Serial.println(rps1);
        //Serial.print("RPS 2: ");
        //Serial.println(rps2);
        last = millis();
    }
    
 /*
 start_time=millis();
 end_time=start_time+1000;
 while(millis()<end_time)
 {
   if(digitalRead(sensor))
   {
    steps=steps+1; 
    while(digitalRead(sensor));
   }

 }
    temp=steps-steps_old;
    steps_old=steps;
    rps=(temp/20);
    Serial.print("RPS: ");
    Serial.println(rps);
    */
    
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


void calculateSpeed(int wheel_sensed, double* wheel_velocity){
    // Time variables
    static unsigned long start_time[WHEEL_SENSOR_NUM] = {0};
    static unsigned long end_time[WHEEL_SENSOR_NUM] = {0};
    
    // Step variables
    static unsigned long steps[WHEEL_SENSOR_NUM] = {0};
    static unsigned long steps_old[WHEEL_SENSOR_NUM] = {0};
    unsigned long diff;
    int index;
    //Serial.println("1");
    // Select which wheels variables will be currently used
    switch(wheel_sensed){
        case LEFT_WHEEL:
            index = 0;
            //Serial.println("2");
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
    //int flag = 0;
    // Measure speed by counting slits during rotation (need to change while? can use flag that determines change in sensor state from 1 to 0 to 1 again)
    if(digitalRead(wheel_sensed)){
        //Serial.println("3");
        steps[index] += 1;
        //flag = 0;
        //Serial.println(steps[index]);
        while(digitalRead(wheel_sensed)); // makes sure same slit is not counted more than once
    }
    /*if(!digitalRead(wheel_sensed)){
        flag = 1;
    }*/

    //Serial.println("4");
    // Calculate wheel speed in mm/ms
    if(end_time[index] - start_time[index] > PERIOD){
        //Serial.println("5");
		diff = steps[index] - steps_old[index];
        double rps = diff / 20.0;
        double velocity = (diff * 3.14 * 65 / 20.0);
        *wheel_velocity = (diff * 1.0 / 20.0);
        //*wheel_velocity = (diff * PI * WHEEL_DIAMETER / NUMBER_OF_SLITS) / (PERIOD);
        //Serial.print("RPS: ");
        //Serial.println(rps);
        //Serial.print("Velocity: ");
        //Serial.println(velocity);
        start_time[index] = end_time[index];
		steps_old[index] = steps[index];
        //Serial.println("6");
    }
    end_time[index] = millis();
    //Serial.println("7");
    //Serial.println(end_time[index] - start_time[index]);
}
