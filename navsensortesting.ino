#define TRIGGER_PIN_1 2
#define ECHO_PIN_1 3

#define TRIGGER_PIN_2 4
#define ECHO_PIN_2 5

#define TRIGGER_PIN_3 6
#define ECHO_PIN_3 7

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(TRIGGER_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  int distanceF = getDistance(TRIGGER_PIN_1, ECHO_PIN_1);
  int distanceR = getDistance(TRIGGER_PIN_2, ECHO_PIN_2);
  int distanceL = getDistance(TRIGGER_PIN_3, ECHO_PIN_3);
  Serial.println("front distance: " + to_string(distanceF) + "\n" + "right distance: " + to_string(distanceR) + "\n" + "left distance: " + to_string(distanceL));

}

int getDistance(int trigger_pin, int echo_pin) {
  // Send ultrasonic pulse
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  // Receive echo and calculate distance
  long duration = pulseIn(echo_pin, HIGH);
  int distance = (duration * 0.0343) / 2;
  return distance;
}
