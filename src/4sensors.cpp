#include <Adafruit_MotorShield.h>
#include <SPI.h>

// set up motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Select which 'port' for each motor (M1, M2, M3, or M4)
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);     // left motor connected to M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);    // right motor connected to M2

// input pins
const int left_sensor_pin_out = 13;
const int left_sensor_pin_in = 9;
const int right_sensor_pin_in = 7;
const int right_sensor_pin_out = 11;

const int button_sensor_pin = 12;
const int led0 = 0;
const int led1 = 1;
const int led2 = 2;
const int left_pressure_switch = 50;
const int right_pressure_sitch = 55;

int left_sensor_state_out;
int left_sensor_state_in;
int right_sensor_state_in;
int right_sensor_state_out;
int left_pressure_switch_state;
int right_pressure_switch_state;
int button_state;
int node_counter = 0;
//bool block_checking = False;
int stopping_count = 0;

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
    leftMotor->setSpeed(255);
    rightMotor->setSpeed(255);
}
void right() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(255);
}
void left() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(255);
    rightMotor->setSpeed(0);
}
void sharp_right_turn() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(1500);
}
void sharp_left_turn() {
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(1400);
}
void one_eighty_turn() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(3000);
}
void drop_block(){
    stop();
    //release pincer and drop block
    one_eighty_turn();
}
void park(){
    //drive forward for two seconds
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(170);
    rightMotor->setSpeed(170);
    delay(2000);
    stop();
    exit(0);
}

// switch statement for different maneuvers
void maneuvers(char c) {
    switch(c) {
        case 'f':
            Serial.println("CASE - Forward");
            forward();
            delay(1000);
            break;
        case 'r':
            Serial.println("CASE - Turn Right");
            sharp_right_turn();
            break;
        case 'l':
            Serial.println("CASE - Turn Left");
            forward();
            delay(1000);
            sharp_left_turn();
            break;
        case 'c':
            //block checking on and go straight
            //block_checking = True;
            Serial.print("Forward, turn on block checking: ");
            //Serial.println(block_checking);
            forward();
            delay(2000);
            break;
        case 'o':
            //block checking off and go straight
            //block_checking = False;
            Serial.print("Forward, turn off block checking: ");
            //Serial.println(block_checking);
            forward();
            delay(2000);
            break;
        case 'd':
            Serial.println("Drop Block");
            drop_block();
            break;
        case 'p':
            Serial.println("Park");
            park();
            break;
        default:
            Serial.println("Default");
            forward();
            delay(2500);
    }

}

//navigation sequences
//sequence 1 - tunnel->pick_up middle block->tunnel:  flfc ...then robot should detect block and pick it up
// the last c turns on block checking and goes straight at node
//sequence 2 - picked up green box block: ordrrp
//sequence 3 - picked up red box block: offrdllp
// the first o turns block checking off and goes straight at node
char nav_seq1[] = "flfc";
char nav_seq_green[] = "ordrrp";
char nav_seq_red[] = "offrdllp";

//set nav sequence to at first be nav_seq1
//char nav_seq[] = nav_seq1;

void navigation(int left_sensor_out, int left_sensor_in, int right_sensor_in, int right_sensor_out) {
    if((left_sensor_out == 0 && left_sensor_in == 1 && right_sensor_in == 0 && right_sensor_out == 0) or
    (left_sensor_out == 1 && left_sensor_in == 0 && right_sensor_in == 0 && right_sensor_out == 0)) {
        Serial.println("right");
        right();
    }
    else if((left_sensor_out == 0 && left_sensor_in == 0 && right_sensor_in == 1 && right_sensor_out == 0) or
    (left_sensor_out == 0 && left_sensor_in == 0 && right_sensor_in == 0 && right_sensor_out == 1)) {
        Serial.println("left");
        left();
    }
    else if(left_sensor_out == 1 && left_sensor_in == 1 && right_sensor_in == 0 && right_sensor_out == 0) { 
        Serial.print("Sharp left, node counter: ");
        Serial.println(node_counter);
        //forward();
        maneuvers(nav_seq1[node_counter]);
        //delay(2000);
        
        node_counter++;
    }
    else if(left_sensor_out == 0 && left_sensor_in == 0 && right_sensor_in == 1 && right_sensor_out == 1) {
        Serial.print("Sharp right, node counter: ");
        Serial.println(node_counter);
        forward();
        maneuvers(nav_seq1[node_counter]);
        //delay(2000);
        node_counter++;
    }
    else if(left_sensor_out == 1 && left_sensor_in == 1 && right_sensor_in == 1 && right_sensor_out == 1) {
        Serial.print("Crossroad, node counter: ");
        Serial.println(node_counter);
        forward();
        maneuvers(nav_seq1[node_counter]);
        //delay(2000);
        node_counter++;
    }
    else {
        forward();
    }
}


void setup() {
    Serial.begin(9600);
    Serial.println("Connected to serial port");

    if (!AFMS.begin()) {    
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    Serial.println("Motor Shield found.");

    delay(3000);

    /*
    leftMotor->setSpeed(150);
    leftMotor->run(FORWARD);
    rightMotor -> setSpeed(150);
    rightMotor -> run(FORWARD);
    */
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    
}

void loop() {

    // ---- OFF BUTTON ----
    button_state = digitalRead(button_sensor_pin);
    // if button is pressed, stop robot
    if(button_state==0){
        Serial.println("Off button pressed, now exiting loop");
        leftMotor->run(RELEASE);
        rightMotor->run(RELEASE);
        exit(0);
    }
    

    // read the light sensor values
    left_sensor_state_out = digitalRead(left_sensor_pin_out);
    left_sensor_state_in = digitalRead(left_sensor_pin_in);
    right_sensor_state_in = digitalRead(right_sensor_pin_in);
    right_sensor_state_out = digitalRead(right_sensor_pin_out);

    Serial.println(left_sensor_state_out);
    Serial.println(left_sensor_state_in);
    Serial.println(right_sensor_state_in);
    Serial.println(right_sensor_state_out);

    navigation(left_sensor_state_out, left_sensor_state_in, right_sensor_state_in, right_sensor_state_out);

    // ---- PRESSURE SWITCHES FOR TUNNEL NAVIGATION ----
    /*
    if (left_pressure_switch_state==1 && right_pressure_switch_state==0) {
        right();
    }
    else if (left_pressure_switch_state=0 && right_pressure_switch_state=1) {
        left();
    }
    */
    
    
    Serial.println("-----------------");

}
