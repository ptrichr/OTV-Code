#include "Enes100.h"
#include "Tank.h"
#include <math.h>

const double NORTH = M_PI/2;
const double SOUTH = -(M_PI/2);
const double MARGIN = M_PI/475;

/* The code inside void setup() runs only once, before the code in void loop(). */
void setup() {
	Enes100.begin("Trial By Fire", FIRE, 3, 8, 9); // Required before you can use any other Enes100 functions.
	Tank.begin(); // Required before you can use any other Tank functions.
	
}

/* The code in void loop() runs repeatedly forever */ 
void loop() { 
	
	Enes100.updateLocation();
	
	bool topStart = (Enes100.location.y > 1.0) ? true : false;
	
	faceSite(topStart);
	
	while(1);  // Circumvent the loop and ensure the above statements only get run once.
	
}

/* This is an example function to make both motors drive
 * at the given power.
 */
void setSpeed(int speed) {
	
	Tank.setLeftMotorPWM(speed);
	Tank.setRightMotorPWM(speed);
	
}

void rotateLeft(int speed) {
	
	Tank.setLeftMotorPWM(-speed);
	Tank.setRightMotorPWM(speed);
		
}

void rotateRight(int speed) {
	
	Tank.setLeftMotorPWM(speed);
	Tank.setRightMotorPWM(-speed);
	
}

void faceDir(double limit) {
	
	bool oriented = false;
	
	do {
		
		Enes100.updateLocation();
		
		if (Enes100.location.theta <= limit + MARGIN && Enes100.location.theta >= limit - MARGIN) {
		
			setSpeed(0);
			oriented =  true;
		
		}
	
		if (Enes100.location.theta < limit - MARGIN) {
		
			rotateLeft(90);
		
		}
	
		if (Enes100.location.theta > limit + MARGIN) {
		
			rotateRight(90);
		
		}
		
	} while (!oriented);
	
}

bool moveToLocation(double curr, double limit) {
	
	
	
}

void faceSite(bool top) {
	
	if (top) {
		
		faceDir(SOUTH);
		
	} else {
		
		faceDir(NORTH);
		
	}
	
}