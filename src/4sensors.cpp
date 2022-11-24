#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <Servo.h>
#include <SharpIR.h>
#include <RunningMedian.h>

//define infrared sensor details
#define ir A1
#define model 20150
SharpIR IR_sensor(model, ir);

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
int IR_state;
int is_block;
int block_type;
int stopping_count = 0;

//block determination variables
int distance_to_block;
int mean_distance;
int current_sum;
int current_threshold;
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
    delay(1100);
}
void sharp_left_turn() {
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(1100);
}
void one_eighty_turn() {
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(250);
    rightMotor->setSpeed(250);
    delay(2700);
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

int block_checking(){
    IR_state = 0; // "run ir sensor"
    if (IR_state<= 5){
        return 1;
    }
    else{
        return 0;
    }

}

/*int block_type_check(){
    current_sum = 0;
    for (cycles=0; cycles <=15; cycles++){
        grab_servo.write(100);
        for (i=0; i<=220;i++){
            current_sum += analogRead(current_pin);
            delay(5);
        }
        grab_servo.write(20);
        delay(1000);
    }

    if (current_sum <= current_threshold)
        return 0;  //low density
    else{
        return 1; //high density
    }
    delay(500);
}*/

// switch statement for different maneuvers
void maneuvers(char c) {
    switch(c) {
        case 'f':
            Serial.println("CASE - Forward");
            forward();
            delay(450);
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
            //is_block = block_checking();
            forward();
            delay(500);
            break;
        case 'o':
            //block checking off and go straight
            //block_checking = False;
            Serial.print("Forward, turn off block checking: ");
            //Serial.println(block_checking);
            forward();
            delay(500);
            break;
        case 'd':
            Serial.println("Drop Block");
            drop_block();
            break;
        case 'p':
            Serial.println("Park");
            park();
            break;
        case 't':
            one_eighty_turn();
        default:
            Serial.println("Default");
            digitalWrite(led_green, HIGH);
            break;
    }

}

//navigation sequences
//sequence 1 - tunnel->pick_up middle block->tunnel:  flfc ...then robot should detect block and pick it up
// the last c turns on block checking and goes straight at node
//sequence 2 - picked up green box block: ordrrp
//sequence 3 - picked up red box block: offrdllp
// the first o turns block checking off and goes straight at node

char nav_seq[] = "lfftf";
char nav_seq_green[] = "ordrrp";
char nav_seq_red[] = "offrdllp";


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
        maneuvers(nav_seq[node_counter]);
        //delay(2000);
        
        node_counter++;
    }
    else if(left_sensor_out == 0 && left_sensor_in == 0 && right_sensor_in == 1 && right_sensor_out == 1) {
        Serial.print("Sharp right, node counter: ");
        Serial.println(node_counter);
        forward();
        maneuvers(nav_seq[node_counter]);
        //delay(2000);
        node_counter++;
    }
    else if(left_sensor_out == 1 && left_sensor_in == 1 && right_sensor_in == 1 && right_sensor_out == 1) {
        Serial.print("Crossroad, node counter: ");
        Serial.println(node_counter);
        forward();
        maneuvers(nav_seq[node_counter]);
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

    //grab_servo.attach(9); //What pin is servo on??? 

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

    //set nav sequence to at first be nav_seq1  -- Can't write nav_seq[]=nav_seq1
    //char nav_seq[] = "lfc";

    
    delay(3000);

    leftMotor->setSpeed(150);
    leftMotor->run(FORWARD);
    rightMotor -> setSpeed(150);
    rightMotor -> run(FORWARD);
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    
    previous_millis = millis();
    start_time = millis();

    //Start by driving forwards for a bit
    forward();
    delay(1000);
    
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
    

    // read the light sensor values
    left_sensor_state_out = digitalRead(left_sensor_pin_out);
    left_sensor_state_in = digitalRead(left_sensor_pin_in);
    right_sensor_state_in = digitalRead(right_sensor_pin_in);
    right_sensor_state_out = digitalRead(right_sensor_pin_out);

    /*Serial.println(left_sensor_state_out);
    Serial.println(left_sensor_state_in);
    Serial.println(right_sensor_state_in);
    Serial.println(right_sensor_state_out);*/

    navigation(left_sensor_state_out, left_sensor_state_in, right_sensor_state_in, right_sensor_state_out);

    // ---- TIME CHECKS FOR WHEN NODES SHOULD HAVE HAPPENED ----
    run_time = current_millis - start_time;
    if (run_time > 3000 && node_counter == 0 ){
        sharp_left_turn();
        node_counter=1;
        Serial.println("Auto turn");
        //digitalWrite(led_orange, HIGH);
    }
    /*else if (run_time > 8000 && run_time < 15000 && node_counter ==1){
        //node_counter++;
        Serial.println("Auto node");
        digitalWrite(led_orange, HIGH);
    }*/


    // ---- PRESSURE SWITCHES FOR TUNNEL NAVIGATION ----
    left_pressure_switch_state = digitalRead(left_pressure_switch);
    right_pressure_switch_state = digitalRead(right_pressure_sitch);

    if (left_pressure_switch_state==0 && right_pressure_switch_state==1) {
        Serial.println("crash");
        leftMotor->run(BACKWARD);
        rightMotor->run(FORWARD);
        leftMotor->setSpeed(250);
        rightMotor->setSpeed(250);
        delay(150);
    }
    else if (left_pressure_switch_state==1 && right_pressure_switch_state==0) {
        Serial.println("left");
        leftMotor->run(FORWARD);
        rightMotor->run(BACKWARD);
        leftMotor->setSpeed(250);
        rightMotor->setSpeed(250);
        delay(150);
    }

    /*
    // ---- BLOCK DETERMINATION ----
    if (is_block == 1){
        block_type = block_type_check();
    }
    if (block_type == 0){
        digitalWrite(led_green, HIGH);
        //char nav_seq[]="offrdllp";
        delay(5000);
        digitalWrite(led_green, LOW);
    }
    else{
        digitalWrite(led_orange, HIGH);
        //char nav_seq[]="offrdllp";
        delay(5000);
        digitalWrite(led_orange, LOW);
    }*/
    
    Serial.println(node_counter);
    Serial.print("1:");
    Serial.println(right_sensor_state_out);
    Serial.print("2:");
    Serial.println(right_sensor_state_in);
    Serial.print("3:");
    Serial.println(left_sensor_state_in);
    Serial.print("4:");
    Serial.println(left_sensor_state_out);
    Serial.println("-----------------");
    //delay(200);

}
