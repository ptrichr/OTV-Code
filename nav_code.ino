#include <math.h>
#include "Enes100.h"

/*
* DISCLAIMER: We used GPT-4 to generate the code that the distance sensor uses to read distance (getDistance)
*/

const double NORTH = M_PI/2;
const double SOUTH = -(double)M_PI/2;
const double EAST = 0;
const double WEST = M_PI;
const double MARGIN = M_PI/50;

// pins for the distance sensors (PWM)

// left
const int TRIGGER_1 = 4;
const int ECHO_1 = 5;

// right
const int TRIGGER_2 = 6;
const int ECHO_2 = 7;

// pins for motor control (PWM)

// left motor
const int EN_A = 8;
const int IN_1 = 9;
const int IN_2 = 10;

// right motor
const int EN_B = 13;
const int IN_3 = 11;
const int IN_4 = 12;

// Topology
// int topA = 0;
// int topB = 0;
// int topC = 0;
// int laserDLeft;
// int laserDRight;
// const uint8_t sensorPinLeft = 8;
// const uint8_t sensorPinRight = 9;

/* The code inside void setup() runs only once, before the code in void loop(). */
void setup() {

	Enes100.begin("Trial By Fire", FIRE, 205, 51, 50); // param: name, mission, aruco, tx, rx
  Serial.begin(9600); // DO NOT CHANGE, ESP8266 BAUD IS 9600

  pinMode(EN_A, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(TRIGGER_1, OUTPUT);
  pinMode(TRIGGER_2, OUTPUT);
  pinMode(ECHO_1, INPUT);
  pinMode(ECHO_2, INPUT);

}

/* The code in void loop() runs repeatedly forever */ 
void loop() { 

  // while(1) {
  //   Serial.println(getDistance(TRIGGER_1, ECHO_1));
  // }
	
  double missionLocation;

  getY();
	
 /*
 * face the mission site
 */

	if (getY() > 1.0) {

		faceDir(SOUTH);

  /*
  the aruco is offset from the body of the OTV, and mission site y 
  is the exact y coordinate of the center of the site, offset accordingly
  */

    missionLocation = Enes100.missionSite.y + 0.11;
		
	} else {
		
    faceDir(NORTH);

    missionLocation = Enes100.missionSite.y - 0.11;
		
	}

  /*
  * move to the mission site
  */
	
	moveToLocation(false, missionLocation);

  /*

  insert mission logic here?

  */

	moveToLocation(false, 1.0);
	faceDir(EAST);

  Enes100.updateLocation();
	
	/* Obstacle avoidance logic */

  /*
  update the condition of this while loop to be 
  when the OTV is outside of the obstacle zone
  instead of while(1)
  */

	do {
		
		if (getDistance(TRIGGER_1, ECHO_1) <= 50 || getDistance(TRIGGER_2, ECHO_2) <= 50) {
			
      setMotorSpeed(0);

			if (getY() < 1.0) {
				faceDir(NORTH);
				moveToLocation(false, 1.0);
				faceDir(EAST);
			} else {
				faceDir(SOUTH);
				moveToLocation(false, 1.0);
				faceDir(EAST);
			}

		} else if (getX() > 3) {
			
			if (getY() <= 1.2) {
				faceDir(NORTH);
				while (getY() < 1.2) {
					setMotorSpeed(100);
				}
				faceDir(EAST);
				setMotorSpeed(100);
			} else {
				setMotorSpeed(100);
			}
			
		} else {
			setMotorSpeed(100);
		}
		
	} while (1);
	
}

/* This is an example function to make both motors drive
 * at the given power.
 */

double getX() {

  Enes100.updateLocation();

  return Enes100.location.x;

}

double getY() {

  Enes100.updateLocation();

  return Enes100.location.y;

}

double getHeading() {

  Enes100.updateLocation();

  return Enes100.location.theta;

}

void setMotorSpeed(int PWMspeed) {

  
  if (PWMspeed == 0) {

    digitalWrite(EN_A, LOW);
    digitalWrite(EN_B, LOW);
    return;

  }

  analogWrite(EN_A, PWMspeed);
  analogWrite(EN_B, PWMspeed);

}

void setMotorDir(int motor, bool forward) {

  // left motor
  if (motor == 1) {

    if (forward) {

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);

    } else {

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);

    }

  // right motor
  } if (motor == 2) {

    if (forward) {

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);

    } else {

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);

    }
    
  }

}

void rotateRight(int PWMspeed) {

  setMotorDir(1, true);
  setMotorDir(2, false);
  setMotorSpeed(PWMspeed);

}

void rotateLeft(int PWMspeed) {

  setMotorDir(1, false);
  setMotorDir(2, true);
  setMotorSpeed(PWMspeed);

}

void faceDir(const double limit) {

  Serial.print("faceDir: ");
  Serial.println(limit);

  Enes100.updateLocation();
  
	do {

    Enes100.println(getHeading());
    Serial.print("getheading(): ");
    Serial.println(getHeading());
	
		if ((getHeading() < limit - MARGIN) || (getHeading() > limit + MARGIN)) {
	
			rotateLeft(120);
      Enes100.println("here1");
      Serial.print("limit1: ");
      Serial.println(limit);
		
		}
	
		// if (getHeading() > limit + MARGIN) {
		
		// 	rotateRight(120);
    //   Enes100.println("here2");
    //   Serial.print("limit2: ");
    //   Serial.println(limit);
		
		// }

    delay(100);
    setMotorSpeed(0);

		
	} while ((getHeading() <= limit + MARGIN) && (getHeading() >= limit - MARGIN));

  setMotorSpeed(0);
  Enes100.println("here0");
  Serial.print("limit0: ");
  Serial.println(limit);

	
}

void moveToLocation(bool xDir, double limit) {

  Enes100.updateLocation()
	
	if (xDir) {
		
		moveX(limit);	
		
	} else {
		
		moveY(limit);
		
	}
	
}

void moveX(double limit) {
	
	do {

    if (getX() > limit + MARGIN) {

      setMotorDir(1, false);
      setMotorDir(2, false);

    }

    if (getX() < limit - MARGIN) {

      setMotorDir(1, true);
      setMotorDir(2, true);

    }
		
		setMotorSpeed(90);
		
	} while ((getX() <= limit + MARGIN) && (getX() >= limit - MARGIN));

  setMotorSpeed(0);
	
}
	
void moveY(double limit) {
	
	do {
		
    if (getY() > limit + MARGIN) {

      setMotorDir(1, false);
      setMotorDir(2, false);

    }

    if (getY() < limit - MARGIN) {

      setMotorDir(1, true);
      setMotorDir(2, true);

    }
		
		setMotorSpeed(90);
		
	} while ((getY() <= limit + MARGIN && getY() >= limit - MARGIN));

  setMotorSpeed(0);
	
}

int getDistance(int trigger_pin, int echo_pin) {

  // Send ultrasonic pulse
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  // Receive echo and calculate distance in cm
  long duration = pulseIn(echo_pin, HIGH);
  int distance = (duration * 0.0343) / 2;
  return distance;

}

// void doMission() {

// }

// int laserDistanceConvert(const uint8_t sensorPin) {

//   int16_t t = pulseIn(sensorPin, HIGH);
 
//   if (t == 0) {
//     // pulseIn() did not detect the start of a pulse within 1 second.
//     Serial.println("timeout");
//   } else if (t > 1850) {
//     // No detection.
//     Serial.println(-1);
//   } else {
//     // Valid pulse width reading. Convert pulse width in microseconds to distance in millimeters.
//     int16_t d = (t - 1000) * 3 / 4;
 
//     // Limit minimum distance to 0.
//     if (d < 0) {
//       d = 0;
//     }

//     return d;
//   }

// }