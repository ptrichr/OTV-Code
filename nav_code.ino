#include "nav_code.h"
#include <Servo.h>

/*
* DISCLAIMER: We used GPT-4 to generate the code that the distance sensor uses to read distance (getDistance)
*/

Servo myservo;
int analogIRVal; //analog readings

void setup() {
 
  // param: aruco, tx, rx
  missionStart(206, 51, 50);
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

  // while(1) {
  //   Serial.println(getDistance(TRIGGER_1, ECHO_1));
  // }
	
  double missionLocation;
	
//  /*
//  * face the mission site
//  */

	if (getY() > 1.0) {

		faceDir(SOUTH);

  /*
  the aruco is offset from the body of the OTV, and mission site y 
  is the exact y coordinate of the center of the site, offset accordingly
  */

  // replace 0.55 with getYMissionLocation()

    missionLocation = 0.55 + 0.11;
		
	} else {
		
    faceDir(NORTH);

    missionLocation = 0.55 - 0.11;
		
	}

  /*
  * move to the mission site
  */
	
	moveToLocation(false, missionLocation);

  /*

  insert mission logic here?

  */

  setMotorDir(1, false);
  setMotorDir(2, false);

	moveToLocation(false, 1.0);

  setMotorDir(1, true);
  setMotorDir(2, true);

	faceDir(EAST);

  updateLocation();
  
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
			
			if (getY() <= 1.5) {
				faceDir(NORTH);
				while (getY() < 1.5) {
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
        reportTop(1);
        break;
      }
    } else if ( (laserDLeft > 75 && laserDLeft < 100) && (laserDRight > 65 && laserDRight < 80) ) {
      topB++;
      if (topB > 2) {
        reportTop(2);
        break;
      }
    } else if ( (laserDLeft > 40 && laserDLeft < 60) && (laserDRight > 60 && laserDRight < 75) ) {
      topC++;
      if (topC > 2) {
        reportTop(3);
        break;
      }
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

  reportFlames(num_flames); 

  num_flames = 0;

  delay(3000);


  // SNUFF THE FLAMES
  myservo.attach(2);

  myservo.write(105); // rotate the motor (<90 means pull snuffers up, >90 means drop snuffers)

  delay(2000); // keep rotating for 5 seconds (5000 milliseconds)

  myservo.write(90); // stop the motor

  delay(1000); // stay stopped

  myservo.write(75); // rotate the motor (<90 means pull snuffers up, >90 means drop snuffers)

  delay(1700);

  myservo.write(90); // stop the motor

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