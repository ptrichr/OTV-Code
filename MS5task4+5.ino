#include <Enes100.h>

#define ARUCO 9
#define TXPIN 2
#define RXPIN 3

float xpos = Enes100.location.x, ypos = Enes100.location.y, orientation = Enes100.location.theta;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Enes100.begin("Trial by Fire", FIRE, ARUCO, TXPIN, RXPIN);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  Enes100.updateLocation();
  Serial.println("x position: " + to_string(xpos) + " y position: " + to_string(ypos) + " angular heading: " + to_string(orientation));
  Enes100.println("x position: " + to_string(xpos) + " y position: " + to_string(ypos) + " angular heading: " + to_string(orientation));

}
