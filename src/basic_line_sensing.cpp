#include <Adafruit_MotorShield.h>
#include <SPI.h>

// set up motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Select which 'port' for each motor (M1, M2, M3, or M4)
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);     // left motor connected to M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);    // right motor connected to M2

// input pins
const int left_sensor_pin = 7;
const int middle_sensor_pin = 9;
const int right_sensor_pin = 11;
const int button_sensor_pin = 12;
const int led0 = 0;
const int led1 = 1;
const int led2 = 2;

int left_sensor_state;
int middle_sensor_state;
int right_sensor_state;
//int button_state;
int crossroad_counter = 0;
int sharp_turn_counter = 0;

void stop() {
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(0);
}
void forward() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(220);
    rightMotor->setSpeed(220);
}
void sharp_right_turn() {
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
}
void sharp_left_turn() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
}
//NOTE - calibrate turns so robot turns 90 degree

//function for testing if sensors are reading correctly
void navigation(int left_sensor, int middle_sensor, int right_sensor) {
    
    if(middle_sensor == 1){
        if(left_sensor == 0 && right_sensor ==0){
            Serial.println("Move forward");
            forward();
        }
        else if(left_sensor == 1 && right_sensor == 0){
            Serial.println("Sharp left - stop");
            stop();
        }
        else if(left_sensor == 0 && right_sensor == 1){
            Serial.println("Sharp right - stop");
            stop();
        }
        else if(left_sensor == 1 && right_sensor == 1){
            //Serial.println("At T-Junction - stop");
            Serial.print("crossroad counter");
            Serial.println(crossroad_counter);
            crossroad_counter++;
            stop();
        }
    }
    else {
        if(left_sensor == 1 && right_sensor == 0){
            Serial.println("Left sensor detected line -> turn right");
            leftMotor->run(BACKWARD);
            rightMotor->run(FORWARD);
            leftMotor->setSpeed(125);
            rightMotor->setSpeed(255);
        }
        else if(left_sensor == 0 && right_sensor == 1){
            Serial.println("Right sensor detected line -> turn left");
            leftMotor->run(FORWARD);
            rightMotor->run(BACKWARD);
            leftMotor->setSpeed(255);
            rightMotor->setSpeed(125);
        }
        else{
            /*
            if(counter>25){
                Serial.println("has lost line");
                leftMotor->run(RELEASE);
                rightMotor->run(RELEASE);
                leftMotor->setSpeed(0);
                rightMotor->setSpeed(0);
                return;
            }
            */
            Serial.println("Nothing detected - Move forward");
            forward();
        }
    }
}


void setup() {
    Serial.begin(9600);
    Serial.println("Connected to serial port");
    Serial.println("Motor Shield found.");

    if (!AFMS.begin()) {    
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    Serial.println("Motor Shield found.");

    leftMotor->setSpeed(150);
    leftMotor->run(FORWARD);
    rightMotor -> setSpeed(150);
    rightMotor -> run(FORWARD);
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}

void loop() {

    //Serial.println("start of loop");

    //button_state = digitalRead(button_sensor_pin);
    // if button is pressed, stop robot
    /*
    if(button_state==1){
        Serial.println("Off button pressed, now exiting loop");
        leftMotor->run(RELEASE);
        rightMotor->run(RELEASE);
        exit(0);
    }
    */

    // read the light sensor values
    left_sensor_state = digitalRead(left_sensor_pin);
    middle_sensor_state = digitalRead(middle_sensor_pin);
    right_sensor_state = digitalRead(right_sensor_pin);

    //Serial.println(left_sensor_state);
    //Serial.println(middle_sensor_state);
    //Serial.println(right_sensor_state);

    navigation(left_sensor_state, middle_sensor_state, right_sensor_state);
    //delay(100);
    
    Serial.println("-----------------");
}

