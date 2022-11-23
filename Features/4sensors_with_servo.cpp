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

Servo myservo;


// input pins
const int left_sensor_pin_out = 7;
const int left_sensor_pin_in = 8;
const int right_sensor_pin_out = 9;
const int right_sensor_pin_in = 11;

const int button_sensor_pin = 12;
const int led0 = 0;
const int led1 = 1;
const int led2 = 2;
const int currentPin = A0;

int left_sensor_state_out;
int left_sensor_state_in;
int right_sensor_state_in;
int right_sensor_state_out;
int button_state;
int node_counter = 0;

bool block_checking = True;

int distance_to_block;
int mean_distance;

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
    delay(5000);
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

void pick_up_block(){
    stop();




    //extend pincer
    //pick up block and check which type
    //if(green box block) -> nav_seq[] = nav_seq_green[]
    //else -> nav_seq[] = nav_seq_red[]

    currentSum = 0;
        for (cycles=0; cycles <= 3; cycles++){
            myservo.write(100);
            for (i = 0; i <= 110; i +=1){
                sumCurrent += analogRead(A0);
            }
            delay(2000);
        }

        Serial.println("scream: " + currentSum);
        if currentSum
        
        
        servo.write(30);

    //reset node counter
    node_counter = 0;
    one_eighty_turn();
}

// switch statement for different maneuvers
void maneuvers(char c) {
    switch(c) {
        case 'f':
            Serial.println("Forward");
            forward();
            break;
        case 's':
            Serial.println("Stop");
            stop();
            break;
        case 'r':
            Serial.println("Turn Right");
            sharp_right_turn();
            break;
        case 'l':
            Serial.println("Turn Left");
            sharp_left_turn();
            break;
        case 'c':
            //block checking on and go straight
            block_checking = True;
            Serial.print("Forward, turn on block checking: ");
            Serial.println(block_checking);
            forward();
            break;
        case 'o':
            //block checking off and go straight
            block_checking = False;
            Serial.print("Forward, turn off block checking: ");
            Serial.println(block_checking);
            forward();
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
char nav_seq[] = nav_seq1;

void navigation(int left_sensor_out, int left_sensor_in, int right_sensor_in, int right_sensor_out) {
    if((left_sensor_out == 0 && left_sensor_in == 1 && right_sensor_in == 0 && right_sensor_out == 0) or
    (left_sensor_out == 1 && left_sensor_in == 0 && right_sensor_in == 0 && right_sensor_out == 0)) {
        //left sensor detected, right motor on
        right();
    }
    else if((left_sensor_out == 0 && left_sensor_in == 0 && right_sensor_in == 1 && right_sensor_out == 0) or
    (left_sensor_out == 0 && left_sensor_in == 0 && right_sensor_in == 0 && right_sensor_out == 1)) {
        //right sensor detected, left motor on
        left();
    }
    else if(left_sensor_out == 1 && left_sensor_in == 1 && right_sensor_in == 0 && right_sensor_out == 0) { 
        Serial.print("Sharp left, node counter: ");
        Serial.println(node_counter);
        //maneuvers(nav_seq[node_counter]);
        stop();
        node_counter++;
    }
    else if(left_sensor_out == 0 && left_sensor_in == 0 && right_sensor_in == 1 && right_sensor_out == 1) {
        Serial.print("Sharp right, node counter: ");
        Serial.println(node_counter);
        //maneuvers(nav_seq[node_counter]);
        stop();
        node_counter++;
    }
    else if(left_sensor_out == 1 && left_sensor_in == 1 && right_sensor_in == 1 && right_sensor_out == 1) {
        //all sensors detecting line, at crossroads 
        Serial.print("Crossroad, node counter: ");
        Serial.println(node_counter);
        //maneuvers(nav_seq[node_counter]);
        stop();
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

    // attatches the servo on pin 8 to the servo object
    myservo.attach(8); 

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
    left_sensor_state_out = digitalRead(left_sensor_pin_out);
    left_sensor_state_in = digitalRead(left_sensor_pin_in);
    right_sensor_state_in = digitalRead(right_sensor_pin_in);
    right_sensor_state_out = digitalRead(right_sensor_pin_out);

    //Serial.println(left_sensor_state_out);
    //Serial.println(left_sensor_state_in);
    //Serial.println(right_sensor_state_in);
    //Serial.println(right_sensor_state_out);

    navigation(left_sensor_state_out, left_sensor_state_in, right_sensor_state_in, right_sensor_state_out);

    // check if there is a block in front when block_checking = True
    if(block_checking==True) {
        Serial.println("CHECKING FOR BLOCK....");

        distance_to_block = IR_sensor.getDistance();
        mean_distance = (distance_to_block/2) + 2;
        Serial.println("Mean Distance to block: " + mean_distance);

        //if mean distance is less than or equal to 7, pick up block
        if(mean_distance <= 7){
            stop();

            for (cycles=0; cycles <= 3; cycles++) {
                myservo.write(30);
                
                //make list (called samples) for holding analog readings, size 110
                RunningMedian samples = RunningMedian(110);
                for (i = 0; i <= 110; i +=1) {
                    current = analogRead(currentPin);
                    samples.add(current);
                }

                delay(2000);
                myservo.write(165);
                delay(2000);

                //getAverage() gives the avearge of the samples list
                Serial.println("Mean reading" + samples.getAverage());
            }
        
            if(samples.getAverage() <= 850) {
                Serial.println("Low density block");
                //set navigation sequence to go to green box
                nav_seq = nav_seq_green;
            }
            else {
                Serial.println("High density block");
                //set navigation sequence to go to red box
                nav_seq = nav_seq_red;
            }

            //reset node counter to zero
            node_counter = 0;
            //turn robot around after picking up block
            one_eighty_turn();
        }
    }

    
    //Serial.println("-----------------");
}
