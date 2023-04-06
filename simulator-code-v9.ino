#include "Enes100.h"
#include "Tank.h"
#include <math.h>

const double NORTH = M_PI/2;
const double SOUTH = -(M_PI/2);
const double MARGIN = M_PI/370;

/* The code inside void setup() runs only once, before the code in void loop(). */
void setup() {
	Enes100.begin("Trial By Fire", FIRE, 3, 8, 9); // Required before you can use any other Enes100 functions.
	Tank.begin(); // Required before you can use any other Tank functions.
	
}

/* The code in void loop() runs repeatedly forever */ 
void loop() { 
	
	Enes100.updateLocation();
	
	bool topStart = (Enes100.location.y > 1.0);
	
	if (topStart) {
		
		faceDir(SOUTH);
		
	} else {
		
		faceDir(NORTH);
		
	}
	
	moveToLocation(false, Enes100.missionSite.y);
	Enes100.updateLocation();
	
	if (topStart) {
		
		faceDir(Enes100.location.theta + NORTH);
		
	} else {
		
		faceDir(Enes100.location.theta + SOUTH);
		
	}
	
	moveToLocation(true, 3.2);
	
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
		
		if (abs(Enes100.location.theta) <= abs(limit) + MARGIN && abs(Enes100.location.theta) >= abs(limit) - MARGIN) {
		
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

void moveToLocation(bool xDir, double limit) {
	
	if (xDir) {
		
		moveX(limit);	
		
	} else {
		
		moveY(limit);
		
	}
	
}

void moveX(double limit) {
	
	bool done = false;
	
	do {
		
		Enes100.updateLocation();
		
		if (Enes100.location.x <= limit + MARGIN && Enes100.location.x >= limit - MARGIN) {
			
			setSpeed(0);
			done = true;
			return;
			
		} 
		
		setSpeed(90);
		
	} while (!done);
	
}
	
void moveY(double limit) {
	
	bool done = false;
	
	do {
		
		Enes100.updateLocation();
		
		if (Enes100.location.y <= limit + MARGIN && Enes100.location.y >= limit - MARGIN) {
			
			setSpeed(0);
			done = true;
			return;
			
		} 
		
		setSpeed(90);
		
	} while (!done);
	
}