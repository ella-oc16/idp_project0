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
int crossroad_counter = 0;
int sharp_turn_counter = 0;

// motor movement functions
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
void right() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(250);
}
void left() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(0);
}
void sharp_right_turn() {
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(2000);
}
void sharp_left_turn() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(2000);
}
void one_eighty_turn() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    //change delay depending on how long it takes for robot to turn around
    delay(3000);
}
void drop_block(){
    //code to drop block
}

// switch statement for different maneuvers
void maneuvers(char c) {
    switch(c) {
        case 'f':
            Serial.println("Case f");
            forward();
            break;
        case 's':
            Serial.println("Case s");
            stop();
            break;
        case 'r':
            Serial.println("Case r");
            sharp_right_turn();
            break;
        case 'l':
            Serial.println("Case l");
            sharp_left_turn();
            break;
        case 'd':
            Serial.println("Case d");
            //block dropping sequence: stop, drop and turn 180
            stop();
            drop_block();
            one_eighty_turn();
            break;
        default:
            Serial.println("Default");
            forward();
    }

}

//navigation sequences
char crossroad_seq[] = "frfrfrfr";
//frfdrfdrfdr
char turn_seq[] = "fffrff";

// navigation logic
void navigation(int left_sensor, int middle_sensor, int right_sensor) {
    if(middle_sensor == 1){
        if(left_sensor == 0 && right_sensor ==0){
            //Serial.println("Move forward");
            forward();
        }
        else if(left_sensor == 1 && right_sensor == 0){
            Serial.print("Sharp left, sharp turn counter: ");
            Serial.println(sharp_turn_counter);
            maneuvers(turn_seq[sharp_turn_counter]);
            sharp_turn_counter++;
        }
        else if(left_sensor == 0 && right_sensor == 1){
            Serial.print("Sharp right, sharp turn counter: ");
            Serial.println(sharp_turn_counter);
            maneuvers(turn_seq[sharp_turn_counter]);
            sharp_turn_counter++;
        }
        else if(left_sensor == 1 && right_sensor == 1){
            //manuever depends on where we are in sequence
            //crossroad_seq[1] will give second manoever to be performed at crossroads
            Serial.print("Crossroad, crossroad counter: ");
            Serial.println(crossroad_counter);
            maneuvers(crossroad_seq[crossroad_counter]);
            crossroad_counter++;
        }
    }
    else {
        if(left_sensor == 1 && right_sensor == 0){
            //Serial.println("Left sensor detected line -> turn right");
            right();
        }
        else if(left_sensor == 0 && right_sensor == 1){
            //Serial.println("Right sensor detected line -> turn left");
            left();
        }
        else{
            // Nothing detected
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

    //Serial.println(left_sensor_state);
    //Serial.println(middle_sensor_state);
    //Serial.println(right_sensor_state);

    navigation(left_sensor_state, middle_sensor_state, right_sensor_state);
    
    //Serial.println("-----------------");
}
