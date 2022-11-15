#include <Adafruit_MotorShield.h>
#include <SPI.h>

//Define ultra sound signal
const int ultrasonic_trigger_pin = 5;
const int ultrasonic_echo_pin = 4;     //check correct pin allocations

int ultrasonic_trigger, ultrasonic_echo;


//Tunnel detection function checks if the tunnel is there and returns 1 if it is
int tunnel_detection() {     //TODO add return value
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
        return 1;
    }

    else {
        return 0;
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
    int is_tunnel, after_tunnel, count;
    is_tunnel = 0;
    after_tunnel = 0;

    if(after_tunnel ==0){     //Runs tunnel check if the tunnel hasn't passed

        if (is_tunnel == 0){    
            is_tunnel = tunnel_detection();
        }
    
        while (is_tunnel == 1){     //while in the tunnel, it drives forward and chacks its still in the tunnel
            count = 0;
            if (count == 0){
                drive_forward();    //uses function from main
                count ++;
            }
            is_tunnel =tunnel_detection();
        }
     
        after_tunnel = 1;    //after passing the tunnel, stop checking for the tunnel

    }

    

}


