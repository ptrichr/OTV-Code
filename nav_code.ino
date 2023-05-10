#include <math.h>
#include "Enes100.h"
#include <Servo.h>

/*
* DISCLAIMER: We used GPT-4 to generate the code that the distance sensor uses to read distance (getDistance)
*/

const double NORTH = M_PI/2;
const double SOUTH = -(double)M_PI/2;
const double EAST = 0;
const double WEST = M_PI;
const double MARGIN = M_PI/10;

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

// Create the servo object
Servo myservo;

// Topology sensors
const uint8_t sensorPinLeft = 30;
const uint8_t sensorPinRight = 31;

// Flame sensors
int analogPinIR1 = A0;
int analogPinIR2 = A1;
int analogPinIR3 = A2;
int analogPinIR4 = A3;
int analogIRVal; //analog readings



/* The code inside void setup() runs only once, before the code in void loop(). */
void setup() {

	Enes100.begin("Trial By Fire", FIRE, 206, 51, 50); // param: name, mission, aruco, tx, rx
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

  pinMode(analogPinIR1, INPUT);
  pinMode(analogPinIR2, INPUT);
  pinMode(analogPinIR3, INPUT);
  pinMode(analogPinIR4, INPUT);

  myservo.detach(); 

}

/* The code in void loop() runs repeatedly forever */ 
void loop() { 
	
  double missionLocation;
  bool bottom;
  int i;

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
	
	
	Enes100.updateLocation();

  setMotorDir(1, true);
  setMotorDir(2, true);
  setMotorSpeed(100);
  delay(5000);
  setMotorSpeed(0);
  Enes100.updateLocation();

  // DO MISSION
  delay(3000);
  // doMission();

  setMotorDir(1, false);
  setMotorDir(2, false);

  setMotorSpeed(100);
  delay(5200);
  setMotorSpeed(0);

  setMotorDir(1, true);
  setMotorDir(2, true);

	
	Enes100.updateLocation();
  delay(100);
  if (getY() < 1.0) {
    faceDir(EAST);
  } else {
    faceDirRight(EAST);
  }
  

	Enes100.updateLocation();
	
	/* Obstacle avoidance logic */

  /*
  update the condition of this while loop to be 
  when the OTV is outside of the obstacle zone
  instead of while(1)
  */

	while (1) {
		
		Enes100.updateLocation();

    if (Enes100.location.y < 1.0) {
			bottom = true;
		} else {
			bottom = false;
		}
		
		while (1) {
			Enes100.updateLocation();

			if ((getDistance(TRIGGER_1, ECHO_1) <= 40 || getDistance(TRIGGER_2, ECHO_2) <= 40) && bottom == true) {
			
				setMotorSpeed(0);
				faceDir(NORTH);
				setMotorDir(1, true);
        setMotorDir(2, true);
        setMotorSpeed(100);
				delay(5000);
        setMotorSpeed(0);
				faceDir(EAST);

			} else if ((getDistance(TRIGGER_1, ECHO_1) <= 40 || getDistance(TRIGGER_2, ECHO_2) <= 40) && bottom == false) {

				setMotorSpeed(0);
				faceDir(SOUTH);
				setMotorDir(1, true);
        setMotorDir(2, true);
        setMotorSpeed(100);
				delay(5000);
        setMotorSpeed(0);
				faceDir(EAST);

			} else {
				break;
			}
		}
    
    if (Enes100.location.x > 2.6) {
			break;
		}
    
    if (Enes100.location.theta > 0 + MARGIN) {
      faceDir(EAST);
    } else if (Enes100.location.theta < 0 - MARGIN) {
      faceDirRight(EAST);
    }

    setMotorDir(1, true);
    setMotorDir(2, true);
		setMotorSpeed(100);
    delay(500);
		
	}  

  /*
  need to add logic here to move OTV to top half of the arena and face the log and go through to end
  */

  while (1) {
    if (Enes100.location.y <= 1.6) {
      setMotorSpeed(0);
      faceDir(NORTH);
      while (Enes100.location.y < 1.6) {
        Enes100.updateLocation();
        setMotorDir(1, true);
        setMotorDir(2, true);
        setMotorSpeed(100);
      }
      setMotorSpeed(0);
      faceDir(EAST);
      setMotorDir(1, true);
      setMotorDir(2, true);
      setMotorSpeed(100);
    } else {
      faceDir(EAST);
      setMotorDir(1, true);
      setMotorDir(2, true);
      setMotorSpeed(100);
    }
  }
	
	while(1);  // Circumvent the loop and ensure the above statements only get run once.
	
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
  setMotorSpeed(PWMspeed + 24);

}

void faceDir(const double limit) {

  Serial.print("faceDir: ");
  Serial.println(limit);
  
  bool oriented = false;

	do {

    Enes100.println(getHeading());
    Serial.print("getheading(): ");
    Serial.println(getHeading());


    if ((getHeading() <= limit)) {

      setMotorSpeed(0);
      oriented = true;
      Enes100.println("here0");
      Serial.print("limit0: ");
      Serial.println(limit);

    }
	
		if ((getHeading() < limit - MARGIN) || (getHeading() > limit)) {
	
			rotateLeft(120);
      Enes100.println("here1");
      Serial.print("limit1: ");
      Serial.println(limit);
		
		}

    delay(50);
    setMotorSpeed(0);

		
	} while (!oriented);

  setMotorSpeed(0);

}

void faceDirRight(const double limit) {

  Serial.print("faceDir: ");
  Serial.println(limit);
  
  bool oriented = false;

	do {

    Enes100.println(getHeading());
    Serial.print("getheading(): ");
    Serial.println(getHeading());


    if (getHeading() >= limit) {

      setMotorSpeed(0);
      oriented = true;
      Enes100.println("here0");
      Serial.print("limit0: ");
      Serial.println(limit);

    }
	
		if ((getHeading() < limit) || (getHeading() > limit + MARGIN)) {
	
			rotateRight(120);
      Enes100.println("here1");
      Serial.print("limit1: ");
      Serial.println(limit);
		
		}

    delay(50);
    setMotorSpeed(0);

		
	} while (!oriented);

  setMotorSpeed(0);

}

void moveToLocation(bool xDir, double limit) {

  setMotorDir(1, true);
  setMotorDir(2, true);
	
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
			
			setMotorSpeed(0);
			done = true;
			return;
			
		} 
		
		setMotorSpeed(90);
		
	} while (!done);
	
}
	
void moveY(double limit) {
	
	bool done = false;
	
	do {
		
		Enes100.updateLocation();
		
		if (Enes100.location.y <= limit + MARGIN && Enes100.location.y >= limit - MARGIN) {
			
			setMotorSpeed(0);
			done = true;
			return;
			
		} 
		
		setMotorSpeed(90);
		
	} while (!done);
	
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

void doMission() {

  // TOPOLOGY
  int topA = 0;
  int topB = 0;
  int topC = 0;
  int laserDLeft;
  int laserDRight;

  while (1) {
    laserDLeft = laserDistanceConvert(sensorPinLeft);
    laserDRight = laserDistanceConvert(sensorPinRight);
    
    if ( (laserDLeft > 40 && laserDLeft < 60) && (laserDRight > 85 && laserDRight < 110) ) {
      topA++;
      if (topA > 2) {
        Enes100.mission(TOPOGRAPHY, TOP_A);
        topA = 0;
        break;
      }
    } else if ( (laserDLeft > 75 && laserDLeft < 100) && (laserDRight > 65 && laserDRight < 80) ) {
      topB++;
      if (topB > 2) {
        Enes100.mission(TOPOGRAPHY, TOP_B);
        topB = 0;
        break;
      }
    } else if ( (laserDLeft > 40 && laserDLeft < 60) && (laserDRight > 60 && laserDRight < 75) ) {
      topC++;
      if (topC > 2) {
        Enes100.mission(TOPOGRAPHY, TOP_C);
        topC = 0;
        break;
      }
    } else {
      Enes100.mission(TOPOGRAPHY, TOP_C);
      break;
    }
  }

  delay(100);

  
  // FLAME DETECT
  int i;
  int num_flames = 0;
  int flame;

  delay(1000);
  
  for (i = A0; i <= A3; i++) {
    flame = infraredFlameDetect(i);
    num_flames += flame;
  }

  Serial.print("Number of flames: ");
  Serial.println(num_flames);
  Enes100.mission(NUM_CANDLES, num_flames);

  num_flames = 0;

  delay(3000);


  // SNUFF THE FLAMES
  myservo.attach(2); // attach the servo to our servo object
  myservo.write(105); // rotate the motor (<90 means pull snuffers up, >90 means drop snuffers)

  delay(2000); // keep rotating for 5 seconds (5000 milliseconds)

  myservo.write(90); // stop the motor

  delay(1000); // stay stopped

  myservo.write(75); // rotate the motor (<90 means pull snuffers up, >90 means drop snuffers)

  delay(1700);

  myservo.write(90); // stop the motor
  myservo.detach(); 

  delay(1000); // stay stopped

}

int infraredFlameDetect(int flameSensor) {

  // Read the analog interface
  analogIRVal = analogRead(flameSensor); 

  if (analogIRVal < 60) {
    Serial.print("Sensor ");
    Serial.print(flameSensor - 53);
    Serial.print(": ");
    Serial.println("Flame detected");
    return 1;
  } else {
    return 0;
  }
  
  Serial.print("Sensor ");
  Serial.print(flameSensor - 53);
  Serial.print(": ");
  Serial.println(analogIRVal); // print analog value to serial
  delay(100);

}

int laserDistanceConvert(const uint8_t sensorPin) {

  int16_t t = pulseIn(sensorPin, HIGH);
 
  if (t == 0) {
    // pulseIn() did not detect the start of a pulse within 1 second.
    Serial.println("timeout");
  } else if (t > 1850) {
    // No detection.
    Serial.println(-1);
  } else {
    // Valid pulse width reading. Convert pulse width in microseconds to distance in millimeters.
    int16_t d = (t - 1000) * 3 / 4;
 
    // Limit minimum distance to 0.
    if (d < 0) {
      d = 0;
    }

    return d;
  }

}