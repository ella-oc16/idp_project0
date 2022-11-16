#include <Adafruit_MotorShield.h>
#include <SPI.h>

// set up motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Select which 'port' for each motor (M1, M2, M3, or M4)
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);     // left motor connected to M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);    // right motor connected to M2

// input pins
const int left_sensor_pin = 11;   //for line sensors
const int middle_sensor_pin = 9;
const int right_sensor_pin = 7;
const int button_sensor_pin = 12;   //for on/off switch
const int ultrasonic_trigger_pin = 5;  //for tunnel detection
const int ultrasonic_echo_pin = 4;     

const int led0 = 0;
const int led1 = 1;
const int led2 = 2;

int left_sensor_state;
int middle_sensor_state;
int right_sensor_state;
int button_state;
int ultrasonic_trigger, ultrasonic_echo; //for tunnel detection

int last_scenario = 0;

int is_tunnel= 0;   //

int i;  // temp counter variables
int position; //to know where on the table the robot is
int turn; //give to navigation to say whether to turn at a junction

//robot motion functions
void drive_forward() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(100);
    rightMotor->setSpeed(100);
}
void drive_backward() {
    leftMotor->run(BACKWARD);
    rightMotor->run(BACKWARD);
    leftMotor->setSpeed(100);
    rightMotor->setSpeed(100);
}
void turn_right() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(75);
    rightMotor->setSpeed(-75);
}
void turn_left() {
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
    leftMotor->setSpeed(-75);
    rightMotor->setSpeed(75);
}


int tunnel_detection(){
    double distance, duration;

    //Send a pulse out of the trigger
    digitalWrite(ultrasonic_trigger_pin, HIGH);
    delay(10);
    digitalWrite(ultrasonic_trigger_pin, LOW);

    // measure duration of pulse from ECHO pin
    duration = pulseIn(ultrasonic_echo_pin, HIGH);

    // calculate the distance
    distance= 0.017 * duration;

    if (distance <20){
        Serial.println("Tunnel detected");
        drive_forward();
        delay(1000);
        return 0;//temp 0, should actually be 1
    }

    else {
        return 0;
    }
}


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
            Serial.println("Straight on at junction");  //changed to assume straight on at junctions. make turning the exception
            leftMotor->run(FORWARD);
            rightMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->setSpeed(200);
        }
        else if(left_sensor == 0 && right_sensor == 1){
            Serial.println("Straight on at Junction");  //changed to assume straight on at junctions. Turning is the exception
            leftMotor->run(FORWARD);
            rightMotor->run(FORWARD);
            leftMotor->setSpeed(200);
            rightMotor->setSpeed(200);
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

    position=0;
    i=0;
    delay(4000);
    
}

void loop() {
    Serial.println(position);

    button_state = digitalRead(button_sensor_pin);
    // if button is pressed, stop robot
    if(button_state==1){
        Serial.println("Off button pressed, now exiting loop");
        leftMotor->run(RELEASE);
        rightMotor->run(RELEASE);
        exit(0);
    }

    switch(position){

        case 0:
            left_sensor_state = digitalRead(left_sensor_pin);
            middle_sensor_state = digitalRead(middle_sensor_pin);
            right_sensor_state = digitalRead(right_sensor_pin);
            if (middle_sensor_state == 0 && left_sensor_state==0 && right_sensor_state==0){ //drives forward until it leaves the box  
                drive_forward();
            }
            else{
                position=1;  
                drive_forward();
                delay(1000);  //drives forward through the box line
            }
            break;

        case 1:
            left_sensor_state = digitalRead(left_sensor_pin);
            middle_sensor_state = digitalRead(middle_sensor_pin);
            right_sensor_state = digitalRead(right_sensor_pin);
            int n = left_sensor_state + right_sensor_state + middle_sensor_state;
            if (n<=2){
                navigation(left_sensor_state, middle_sensor_state, right_sensor_state);
            }
            else{
                turn_left();
                delay(2000);
                position = 2;
            }

        case 2:   //between joining the track and the tunnel, check for tunnel
            left_sensor_state = digitalRead(left_sensor_pin);
            middle_sensor_state = digitalRead(middle_sensor_pin);
            right_sensor_state = digitalRead(right_sensor_pin);
            navigation(left_sensor_state, middle_sensor_state, right_sensor_state);
            is_tunnel = tunnel_detection();
            if (is_tunnel == 1){
                position == 3;
            }
            break;
        
        case 3:    //while in the tunnel, check still in tunnel and drive forward
            while (is_tunnel == 1){
                navigation();  //when no line, navigation drives forward
                is_tunnel = tunnel_detection();
            }
            position  = 4;
            break;

        default:
            Serial.println("Else");
            left_sensor_state = digitalRead(left_sensor_pin);
            middle_sensor_state = digitalRead(middle_sensor_pin);
            right_sensor_state = digitalRead(right_sensor_pin);
            navigation(left_sensor_state, middle_sensor_state, right_sensor_state);

    } 
          

    
    /*
    //Serial.println("start of loop");

    

    //check if in tunnel
   /* is_tunnel = tunnel_detection();
    if (is_tunnel == 1){
        drive_forward();
    }*/
   
    
    Serial.println("-----------------");
}
