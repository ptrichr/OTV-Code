#include <Enes100.h>

// angular directions of the arena
#define NORTH (M_PI)/2
#define SOUTH -(NORTH)
#define EAST 0
#define WEST_1 M_PI
#define WEST_2 -(WEST_1)

// pins for communicating with the overhead vision system
#define TX_PIN 22
#define RX_PIN 24
#define MARKER_ID

// pins for the distance sensors (PWM)
#define TRIGGER_PIN_1 2
#define ECHO_PIN_1 3

#define TRIGGER_PIN_2 4
#define ECHO_PIN_2 5

#define TRIGGER_PIN_3 6
#define ECHO_PIN_3 7

// pins for motor control (PWM)
#define MOTOR_A_PIN_1 8
#define MOTOR_A_PIN_2 9

#define MOTOR_B_PIN_1 10
#define MOTOR_B_PIN_2 11

// begins the mission
Enes100.begin("Trial By Fire", FIRE, MARKER_ID, TX_PIN, RX_PIN);

void setup() {
  /*
    all of the navigation code required to get to the open zone. this will only run once so it makes sense to
    separate this code from code that needs to be run multiple times
  */

  // initializing the pin modes for the ultrasonic sensor pins
  pinMode(TRIGGER_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIGGER_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
  pinMode(TRIGGER_PIN_3, OUTPUT);
  pinMode(ECHO_PIN_3, INPUT);

  // initializing the pin modes for motor control
  pinMode(MOTOR_A_PIN_1, OUTPUT);
  pinMode(MOTOR_A_PIN_2, OUTPUT);
  pinMode(MOTOR_B_PIN_1, OUTPUT);
  pinMode(MOTOR_B_PIN_2, OUTPUT);

  Serial.begin(115200);

  // get the starting position of the OTV
  Enes.updatePosition();

  // initializes 3 floats with the x position, y position and angular orientation
  float xpos = Enes100.location.x, ypos = Enes100.location.y, apos = Enes100.location.theta

  // boolean corresponding to whether or not the OTV is starting at the top position: true if it is in the top position, and false otherwise
  bool startingPos = (ypos > 1.0) ? true : false;

  // makes a call to faceSite() which will orient the OTV towards the mission site depending on its starting position
  faceSite(startingPos, apos);


}

void loop() {
  /*
    all of the code required to navigate through the open zone, including avoiding obstacles
  */
  
  // implement GPT-4 generated template

}

/*
  this method orients the OTV towards the mission site depending on its starting position
*/
void faceSite(bool top, float orientation) {

  if (top) {

    while (orientation != SOUTH) {

      // rotate the OTV to exactly -pi/2 radians so it faces the objective site

      if (orientation > SOUTH) {

        // rotate clockwise

      } else {

        // rotate counter clockwise

      }

      Enes.updateLocation();

    }
    
  } else {

    while (orientation != NORTH) {

      // rotate the OTV to exactly pi/2 radians so it faces the objective site

      if (orientation < NORTH) {

        // rotate counter clockwise

      } else {

        // rotate clockwise

      }

      Enes.updateLocation();

    }
  
  }

}

/*
  this method returns the distance readings from the sensor with the pins that correspond to
  trigger_pin and echo_pin
*/
int getDistance(int trigger_pin, int echo_pin) {
  // Send ultrasonic pulse
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  // Receive echo and calculate distance
  long duration = pulseIn(echo_pin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}
