#include <Adafruit_MotorShield.h>
#include <SPI.h>

//Define ultra sound signal
const int ultrasonic_trigger_pin = 5;
const int ultrasonic_echo_pin = 4;     //check correct pin allocations

int ultrasonic_trigger, ultrasonic_echo;


//Tunnel detection function checks if the tunnel is there and returns 1 if it is
void tunnel_detection() {     //TODO add return value
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
    }

}

void setup(){
    Serial.begin(9600);
    Serial.println("Start");
    // configure the trigger pin to output mode
    pinMode(ultrasonic_trigger_pin, OUTPUT);
    // configure the echo pin to input mode
    pinMode(ultrasonic_echo_pin, INPUT);
    Serial.println("Setup done");
}


void loop(){
    Serial.println("loop");
    tunnel_detection();
    
    delay(1000);
}


