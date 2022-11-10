#include <Adafruit_MotorShield.h>
#include <SPI.h>

//4 (echo), 5 (trigger) - pins for ultrasonic sensor

//TODO - make moving average for reaing servo outputs

//only send command to robot if changiing speed so that speed of loop is as quick as possible 
//i,e, do minimum number of actions per loop

// set up motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Select which 'port' for each motor (M1, M2, M3, or M4)
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);     // left motor connected to M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);    // right motor connected to M2

// sensor input pins
const int left_sensor_pin = 7;
const int middle_sensor_pin = 9;
const int right_sensor_pin = 11;

int left_sensor_state;
int middle_sensor_state;
int right_sensor_state;

int speed = 110;

void drive_forward() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(150);
    rightMotor->setSpeed(150);
}
void drive_backward() {
    leftMotor->run(BACKWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(150);
    rightMotor->setSpeed(150);
}
void turn_right() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(150);
    rightMotor->setSpeed(50);
}
void turn_left() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(100);
    rightMotor->setSpeed(250);
}


//function for testing if sensors are reading correctly
void read_sensors(int left_sensor, int middle_sensor, int right_sensor) {

    //if reading has changed, then do something
    //else pass (dont send a command to engine)
    
    if(right_sensor == 0 && left_sensor == 0 && middle_sensor == 0) {
        Serial.println("No sensors detect line");
        drive_forward();
        // assign to case number
        // case = 1;
        // create a temp variable holder that holds the last case number
        // if case number = last number -> dont change anything
    }
    if(right_sensor == 0 && left_sensor == 0 && middle_sensor == 1) {
        Serial.println("Middle sensor detetcing line -> go straight");
        drive_forward();
        
    }

    if(right_sensor == 1 && left_sensor == 0 && middle_sensor == 0) {
        Serial.println("Right sensor detecting line -> turn right");
        turn_left();
    }
    if(right_sensor == 0 && left_sensor == 1 && middle_sensor == 0) {
        Serial.println("Left sensor detecting line -> turn left");
        turn_right();
    }
    if(right_sensor == 1 && left_sensor == 0 && middle_sensor == 1) {
        Serial.println("At right turn -> turn right or continue?");
    }
    if(right_sensor == 0 && left_sensor == 1 && middle_sensor == 1) {
        Serial.println("At left turn -> turn left or continue?");
    }
    if(right_sensor == 1 && left_sensor == 1 && middle_sensor == 1) {
        Serial.println("All sensors detecting line -> either at a T-junction or crossroads");
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
    // turn on motor
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);

    // wait 3 seconds
    //delay(3000);
}

void loop() {
    uint8_t i;

    //Serial.println("start of loop");

    // read the sensor values
    left_sensor_state = digitalRead(left_sensor_pin);
    middle_sensor_state = digitalRead(middle_sensor_pin);
    right_sensor_state = digitalRead(right_sensor_pin);

    read_sensors(left_sensor_state, middle_sensor_state, right_sensor_state);

    //Serial.println(left_sensor_state);
    //Serial.println(middle_sensor_state);
    //Serial.println(right_sensor_state);
    //delay(500);

    //leftMotor->run(FORWARD);
    //rightMotor->run(FORWARD);
    /*
    if(right_sensor_state == 0 && left_sensor_state == 0 && middle_sensor_state == 0) {
        Serial.println("Middle sensor detecting line -> move forward");
        //delay(2000);

        for (i=0; i<255; i++) {
            leftMotor->setSpeed(i);
            rightMotor->setSpeed(i);
            delay(10);
        }
    
    }
    */
    /*


    // meet a left turn - left and middle sensor on
    // TODO - how do we decide if we want to turn or continue straight
    if(right_sensor_state == 0 && left_sensor_state == 1 && middle_sensor_state == 1) {
        Serial.println("LEFT TURN DETECTED -> turn left or continue straight?");
        delay(2000);

        //if x turn left, else continue straight

        //set left motor to be on only
        //leftMotor->setSpeed(speed)
        //rightMotor->setSpeed(0)
    }
    
    // meet a right turn - right and middle sensor on
    if(right_sensor_state == 1 && left_sensor_state == 0 && middle_sensor_state == 0) {
        Serial.println("RIGHT TURN DETECTED -> turn right or continue straight?");
        delay(2000);

        //if x turn right, else continue straight

        //set left motor to be on only
        //leftMotor->setSpeed(speed)
        //rightMotor->setSpeed(0)
    }
    */
    

   Serial.println("-----------------");
}

//note - releasing the motors causes jittery response, go through motor documentation
