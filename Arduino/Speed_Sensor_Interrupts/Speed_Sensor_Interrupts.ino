// Wheel speed sensors (digital)
// Hardware interrupt pins: 2 and 3
// Pin change interrupt: A0 (PORT C)
#define LEFT_WHEEL 2
#define RIGHT_WHEEL 3
#define BACK_WHEELS A1
// Motor driver
#define IN1 5
#define IN2 6
#define IN3 7
#define IN4 8
#define EN1 9 // PWM
#define EN2 10 // PWM
// Constants
#define PI 3.1415926535


#define WHEEL_SENSOR_NUM 3


// ############################# NEW OBJECTS ######################################
#define NUMBER_OF_SLITS 20.0 // number of slits of encoder wheel (because we interrupt on change, we double the number of slits)
#define WHEEL_DIAMETER 65.0 // in mm (changed to add decimal point)
#define PERIOD 100.0 // (changed to add decimal point)

//volatile unsigned long steps[WHEEL_SENSOR_NUM] = {0};
volatile unsigned int steps1 = 0;
volatile unsigned int steps2 = 0;
volatile unsigned int steps3 = 0;

double rps1 = 0;
double rps2 = 0;
double rps3 = 0;
unsigned long last = 0;

void setup() { 
    Serial.begin(9600);
    Serial.println("START:");
    pinMode(LEFT_WHEEL, INPUT_PULLUP);
    pinMode(RIGHT_WHEEL, INPUT_PULLUP);
    pinMode(BACK_WHEELS, INPUT_PULLUP);
    // Hardware interrupts
    attachInterrupt(digitalPinToInterrupt(LEFT_WHEEL), leftIncrement, CHANGE);
    attachInterrupt(digitalPinToInterrupt(RIGHT_WHEEL), rightIncrement, CHANGE);
    // Pin change interrupt
    PCICR |= B00000010; // Pin Change Interrupt Control Register, setting bit 1 means we enabled port C interrupt
    PCMSK1 |= B00000010; // Pin Change Mask 1 -> We are enabling pin A1 to activate the PORT C ISR
}

void loop() {
  // put your main code here, to run repeatedly:
    calculateSpeed(LEFT_WHEEL, &rps1);
    //calculateSpeed(RIGHT_WHEEL, &rps2);
    //calculateSpeed(BACK_WHEELS, &rps3);
    
    if(millis() - last > 100){
        Serial.print("RPS 1: ");
        Serial.println(rps1);
        //Serial.print("RPS 2: ");
        //Serial.println(rps2);
        //Serial.print("RPS 3: ");
        //Serial.println(rps3);
        last = millis();
    }
    /*if(millis() - last > 1000){
        Serial.print("STEPS 1: ");
        Serial.println(steps1);
        Serial.print("STEPS 2: ");sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
        Serial.print("STEPS 3: ");
        Serial.println(steps3);
        last = millis();
    }*/
}

void leftIncrement(){
    //steps[0]++;
    steps1++;
}

void rightIncrement(){
    //steps[1]++;
    steps2++;
}

ISR(PCINT1_vect){
    //steps[2]++;
    steps3++;
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

    //Serial.println("4");
    // Calculate wheel speed in mm/ms
    if(end_time[index] - start_time[index] > PERIOD){
        // Steps update
        noInterrupts();
        steps[0] = steps1;
        steps[1] = steps2;
        steps[2] = steps3;
        interrupts();
        //Serial.println("5");
        //noInterrupts();
        //steps[index] = steps_interrupt[index];
        //interrupts();
		diff = steps[index] - steps_old[index];
        //Serial.print("STEPS: ");
        //Serial.print(steps[index]);
        //Serial.print(" STEPS_OLD: ");
        //Serial.println(steps_old[index]);
        double rps = diff / 20.0;
        double velocity = (diff * 3.14 * 65 / 20.0);
        //Serial.println(diff);
        *wheel_velocity = (diff * 1.0 / 40.0) / (PERIOD / 1000);
        //*wheel_velocity = (diff * PI * WHEEL_DIAMETER / (NUMBER_OF_SLITS * 2)) / (PERIOD);
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