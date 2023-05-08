#include <Servo.h>

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


void setup() {
  // put your setup code here, to run once:
  pinMode(analogPinIR1, INPUT);
  pinMode(analogPinIR2, INPUT);
  pinMode(analogPinIR3, INPUT);
  pinMode(analogPinIR4, INPUT);

  myservo.attach(2); // attach the servo to our servo object
  myservo.write(90); 

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  doMission();
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
        Serial.println("Topography: A");
        topA = 0;
        break;
      }
    } else if ( (laserDLeft > 75 && laserDLeft < 100) && (laserDRight > 65 && laserDRight < 80) ) {
      topB++;
      if (topB > 2) {
        Serial.println("Topography: B");
        topB = 0;
        break;
      }
    } else if ( (laserDLeft > 40 && laserDLeft < 60) && (laserDRight > 60 && laserDRight < 75) ) {
      topC++;
      if (topC > 2) {
        Serial.println("Topography: C");
        topC = 0;
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

  Serial.print("Number of flames: ");
  Serial.println(num_flames);

  num_flames = 0;

  delay(3000);


  // SNUFF THE FLAMES
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
