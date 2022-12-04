#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <Servo.h>
#include <SharpIR.h>
#include <RunningMedian.h>

//define infrared sensor details
#define ir A1
#define model 1080
//SharpIR IR_sensor(model, ir);
SharpIR IR_sensor(ir, model);

// set up motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Select which 'port' for each motor (M1, M2, M3, or M4)
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);     // left motor connected to M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);    // right motor connected to M2

Servo grab_servo;

// input pins
const int led_yellow = 0;
const int led_green = 1;
const int led_orange = 2;

const int left_sensor_pin_out = 6;
const int left_sensor_pin_in = 5;
const int right_sensor_pin_in = 3;
const int right_sensor_pin_out = 4;

const int left_pressure_switch = 8;
const int right_pressure_sitch = 9;

const int button_sensor_pin = 11;

const int servo_pin = 12;
const int IR_sensor_pin = A1;
const int current_pin = A0;

unsigned long current_millis;
unsigned long previous_millis;
const unsigned long interval = 250;
unsigned long start_time, run_time;

int led_yellow_state;

int left_sensor_state_out;
int left_sensor_state_in;
int right_sensor_state_in;
int right_sensor_state_out;
int left_pressure_switch_state;
int right_pressure_switch_state;
int button_state;
int node_counter = 0;
int block_type;
bool block_checking = false;

//block determination variables
int distance_to_block;
int current_sum;
int current_threshold = 100;
int pos = 0;
int current = 0;
int cycles = 0;
int i = 0;

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
    leftMotor->setSpeed(200);
    rightMotor->setSpeed(200);
}
void backward() {
    leftMotor->run(BACKWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(180);
    rightMotor->setSpeed(180);
}
void right() {
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(255);
}
void left() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(255);
    rightMotor->setSpeed(0);
}
void sharp_right_turn() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(1450);
}
void sharp_left_turn() {
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(1450);
}
void one_eighty_turn() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(2500);
}

void drop_block(){
    stop();
    grab_servo.write(100);
    delay(500);
}
void park(){
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(1250);
    stop();
    delay(1000);
    forward();
    delay(1800);
    stop();
    exit(0);
}

// to get to first block: forward, rigth, forward, turn checking on & move forward (frfc)
// if green (low density): don't change node counter
// if red (high density): change node_counter = 9, so goes to correct place in sequence
char nav_seq[] = "frfcffdrrpffdp";

// switch statement for different maneuvers
bool maneuvers(char c) {
    bool block_checking_status = false;
    
    switch(c) {
        case 'f':
            //FORWARD
            forward();
            delay(350);
            break;
        case 'r':
            //TURN RIGHT
            forward();
            delay(1500);
            sharp_right_turn();
            break;
        case 'l':
            //TURN LEFT
            forward();
            delay(1700);
            sharp_left_turn();
            break;
        case 'c':
            //MOVE FORWARD & TURN ON IR SENSOR
            forward();
            delay(450);
            block_checking_status = true;
            break;
        case 'd':
            //DROP BLOCK
            forward();
            delay(1500);
            sharp_right_turn();
            forward();
            delay(2200);
            drop_block();
            delay(1000);
            backward();
            delay(2600);
            stop();
            leftMotor->run(FORWARD);
            rightMotor->run(BACKWARD);
            leftMotor->setSpeed(250);
            rightMotor->setSpeed(250);
            delay(1050);
            stop();
            delay(2000);
            break;
        case 'p':
            //PARK
            forward();
            delay(1800);
            park();
            break;
        default:
            Serial.println("Default");
            digitalWrite(led_green, HIGH);
            break;
    }
    return block_checking_status;
}

bool navigation_short(int L_outer, int L_inner, int R_inner, int R_outer, bool b_check) {
    //boolean holder variable, b_check only changes if maneouvers changes it
    bool block_checking_status = b_check;
    
    if((L_outer == 0 && L_inner == 1 && R_inner == 0 && R_outer == 0) or
    (L_outer == 1 && L_inner == 0 && R_inner == 0 && R_outer == 0)) {
        right();
    }
    else if((L_outer == 0 && L_inner == 0 && R_inner == 1 && R_outer == 0) or
    (L_outer == 0 && L_inner == 0 && R_inner == 0 && R_outer == 1)) {
        left();
    }
    else if((L_outer == 1 && L_inner == 1 && R_inner == 0 && R_outer == 0) or 
    (L_outer == 0 && L_inner == 0 && R_inner == 1 && R_outer == 1) or 
    (L_outer == 1 && L_inner == 1 && R_inner == 1 && R_outer == 1)) { 
        digitalWrite(led_green, HIGH);
        delay(200);
        digitalWrite(led_green, LOW);
        Serial.print("At node number... ");
        Serial.println(node_counter);
        block_checking_status = maneuvers(nav_seq[node_counter]);
        node_counter++;
    }
    else {
        forward();
    }

    return block_checking_status;
}

void pressure_switch_navigation(int L, int R) {
    if(L==0 && R==1){
        //turn left if right pressure sensor detects wall
        leftMotor->run(BACKWARD);
        rightMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->setSpeed(200);
        delay(350);
    }
    else if(L==1 && R==0){
        //turn right if left pressure sensor detects
        leftMotor->run(FORWARD);
        rightMotor->run(BACKWARD);
        leftMotor->setSpeed(200);
        rightMotor->setSpeed(200);
        delay(350);
    }
}

int block_type_check(){
    current_sum = 0;
    
    //squeeze the block 3 times
    for (cycles=0; cycles <=2; cycles++){
        grab_servo.write(20);
        for (i=0; i<=1000;i++){
            current_sum += analogRead(current_pin);
        }
        grab_servo.write(160);
        delay(500);
    }
    Serial.println(current_sum);

    if (current_sum <= current_threshold) {
        return 0;  //low density
    } else {
        return 1; //high density
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

    grab_servo.attach(servo_pin); 

    pinMode(led_orange, OUTPUT);
    pinMode(led_green, OUTPUT);
    pinMode(led_yellow, OUTPUT);
    pinMode(left_pressure_switch_state, INPUT);
    pinMode(right_pressure_sitch, INPUT);
    pinMode(right_sensor_pin_in, INPUT);
    pinMode(right_sensor_pin_out, INPUT);
    pinMode(left_sensor_pin_in, INPUT);
    pinMode(left_sensor_pin_out, INPUT);
    pinMode(button_sensor_pin, INPUT);

    //INITIAL CHECK OF LINE SENSOR READINGS
    left_sensor_state_out = digitalRead(left_sensor_pin_out);
    left_sensor_state_in = digitalRead(left_sensor_pin_in);
    right_sensor_state_in = digitalRead(right_sensor_pin_in);
    right_sensor_state_out = digitalRead(right_sensor_pin_out);
    
    if(left_sensor_state_out==0 && left_sensor_state_in==0 && right_sensor_state_in==0 && right_sensor_state_out==0){
        Serial.println("Sensors reading correctly");
        digitalWrite(led_green, HIGH);
        delay(2500);
        digitalWrite(led_green, LOW);
    }
    else {
        Serial.println("Sensors reading incorrectly");
        digitalWrite(led_orange, HIGH);
        delay(2500);
        digitalWrite(led_orange, LOW);
    }

    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    
    previous_millis = millis();
    start_time = millis();
}

void loop() {

    // ---- OFF BUTTON ----
    button_state = digitalRead(button_sensor_pin);
    // if button is pressed, stop robot and exit loop
    if(button_state==1){
        Serial.println("Off button pressed, now exiting loop");
        leftMotor->run(RELEASE);
        rightMotor->run(RELEASE);
        exit(0);
    }
    
    // ---- LED BLINKING ---- 
    current_millis = millis();
    if (current_millis - previous_millis >= interval){
         // if the LED is off turn it on and vice-versa:
        if (led_yellow_state == LOW) {
            led_yellow_state = HIGH;
        } else {
            led_yellow_state = LOW;
        }
        previous_millis = current_millis;
        digitalWrite(led_yellow, led_yellow_state);
    }
    
    // ---- LIGHT SENSORS AND NAVIGATION ----
    left_sensor_state_out = digitalRead(left_sensor_pin_out);
    left_sensor_state_in = digitalRead(left_sensor_pin_in);
    right_sensor_state_in = digitalRead(right_sensor_pin_in);
    right_sensor_state_out = digitalRead(right_sensor_pin_out);
    
    //assign the boolean return given by nav function to 'block_checking'
    block_checking = navigation_short(left_sensor_state_out, left_sensor_state_in, right_sensor_state_in, right_sensor_state_out, block_checking);

    // ---- PRESSURE SWITCHES FOR TUNNEL NAVIGATION ----
    left_pressure_switch_state = digitalRead(left_pressure_switch);
    right_pressure_switch_state = digitalRead(right_pressure_sitch);
    pressure_switch_navigation(left_pressure_switch_state, right_pressure_switch_state);

    // ---- READING IR SENSOR WHEN BLOCK CHECKING IS TRUE ----
    if(block_checking==true){

        distance_to_block = (analogRead(IR_sensor_pin))/26;

        if(distance_to_block > 21) {
            Serial.println("Block close enough, pick it up");
            forward();
            delay(400);
            stop();
            
            block_type = block_type_check();
            if(block_type==0) {
                //Low density
                Serial.println("Low density block");
                digitalWrite(led_green, HIGH);
                delay(4000);
                digitalWrite(led_green, LOW);
            }
            else {
                //High density
                Serial.println("High density block");
                digitalWrite(led_orange, HIGH);
                delay(4000);
                digitalWrite(led_orange, LOW);
                node_counter = 10;
            }

            //turn off block checking so doesnt check same block again
            block_checking = false;

            //hold onto block
            grab_servo.write(160);
            forward();
            delay(1500);
        }
    }    
}
