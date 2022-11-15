// Include libraries:
#include <Adafruit_MotorShield.h>

// global variables:
// Select which 'port' for each motor (M1, M2, M3, or M4)
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);     // left motor connected to M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);    // right motor connected to M2

// sensor input pins
const int left_sensor_pin = 7;
const int right_sensor_pin = 9;
const int top_sensor_pin = 11;
//const int bottom_sensor_pin = ..;

// will either be HIGH (1) or LOW (0)
int left_sensor_state;
int top_sensor_state;
int right_sensor_state;
int bottom_sensor_state;

int speed = 110;

void setup() {
    Serial.begin(9600);
    Serial.println("Connected to serial port");

    if (!AFMS.begin()) {
        Serial.Println("Could not find Motor Shield. Check wiring.");
        while(1);
    }
    Serial.println("Motor Shield found.");

    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    Serial.println("Set motors to move forward");

    leftMotor->setSpeed(speed);
    rightMotor->setSpeed(speed);
    Serial.println("Set motor speeds");

    // turn on motors
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);

    // wait 3 seconds
    delay(3000);
}

void loop() {

    // read the sensor values
    left_sensor_state = digitalRead(left_sensor_pin);
    top_sensor_state = digitalRead(top_sensor_pin);
    right_sensor_state = digitalRead(right_sensor_pin);

    // TODO - create logic to decide if want to be in forward or reverse mode
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);

    // if top and bottom sensors on, right and left sensors off, move forward
    if(right_sensor_state = 0 && left_sensor_state = 0 && top_sensor_state = 1) {
        Serial.println("All sensors detecting line -> move forward");

        //set motors to both be on, same speed
        leftMotor->setSpeed(speed);
        rightMotor->setSpeed(speed);
    }

    // if top and bottom sensors on and then right sensor comes on, turn right to correct path
    if(right_sensor_state = 1 && left_sensor_state = 0 && top_sensor_state = 1) {
        Serial.println("Right sensor detecting line -> turn right to correct path");

        //set right motor to be on only
        leftMotor->setSpeed(0);
        rightMotor->setSpeed(speed);
    }

    // if top and bottom sensors on and then leftf sensor comes on, turn left to correct path
    if(right_sensor_state = 0 && left_sensor_state = 1 && top_sensor_state = 1) {
        Serial.println("Left sensor detecting line -> turn left to correct path");

        //set left motor to be on only
        leftMotor->setSpeed(speed);
        rightMotor->setSpeed(0);
    }

    // T-JUNCTION: if top sensor off, all other sensors on
    // decide if you want to turn left or right
    if(right_sensor_state = 1 && left_sensor_state = 1 && top_sensor_state = 0 ){
        Serial.println('Bottom three sensors detecting line -> at T-junction');
    }
        //TODO - how do we decide whether to go left or right at a T-JUNCTION
}

