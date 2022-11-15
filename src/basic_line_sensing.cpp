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
int button_state;

//function for testing if sensors are reading correctly
void navigation(int left_sensor, int middle_sensor, int right_sensor) {
    
    if(middle_sensor == 1){
        if(left_sensor == 0 && right_sensor ==0){
            Serial.println("Move forward");
            leftMotor->run(FORWARD);
            rightMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->setSpeed(200);
        }
        else if(left_sensor == 1 && right_sensor == 0){
            Serial.println("Sharp left");
            leftMotor->run(FORWARD);
            rightMotor->run(FORWARD);
            leftMotor->setSpeed(0);
            rightMotor->setSpeed(200);
        }
        else if(left_sensor == 0 && right_sensor == 1){
            Serial.println("Sharp right");
            leftMotor->run(FORWARD);
            rightMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->setSpeed(0);
        }
        else if(left_sensor == 1 && right_sensor == 1){
            Serial.println("At T-Junction - stop");
            leftMotor->run(RELEASE);
            rightMotor->run(RELEASE);
            leftMotor->setSpeed(0);
            rightMotor->setSpeed(0);
        }
    }
    else {
        if(left_sensor == 1 && right_sensor == 0){
            Serial.println("Left");
            leftMotor->run(FORWARD);
            rightMotor->run(BACKWARD);
            leftMotor->setSpeed(150);
            rightMotor->setSpeed(150);
        }
        else if(left_sensor == 0 && right_sensor == 1){
            Serial.println("Right");
            leftMotor->run(BACKWARD);
            rightMotor->run(FORWARD);
            leftMotor->setSpeed(150);
            rightMotor->setSpeed(150);
        }
        else if(left_sensor == 0 && right_sensor == 0){
            Serial.println("Nothing being detected");
            leftMotor->run(RELEASE);
            rightMotor->run(RELEASE);
            leftMotor->setSpeed(0);
            rightMotor->setSpeed(0);
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

    //make robot go foward in straight line until it reaches the line
    /*
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(120);
    rightMotor->setSpeed(120);
    delay(7000);                     //adjust delay so robot moves by required amount
    */
}

void loop() {

    //Serial.println("start of loop");

    button_state = digitalRead(button_sensor_pin);
    // if button is pressed, stop robot
    if(button_state==1){
        Serial.println("Off button pressed, now exiting loop");
        leftMotor->run(RELEASE);
        rightMotor->run(RELEASE);
        exit(0);
    }

    // read the light sensor values
    left_sensor_state = digitalRead(left_sensor_pin);
    middle_sensor_state = digitalRead(middle_sensor_pin);
    right_sensor_state = digitalRead(right_sensor_pin);

    Serial.println(left_sensor_state);
    Serial.println(middle_sensor_state);
    Serial.println(right_sensor_state);

    navigation(left_sensor_state, middle_sensor_state, right_sensor_state);
    //delay(100);
    
    Serial.println("-----------------");
}

