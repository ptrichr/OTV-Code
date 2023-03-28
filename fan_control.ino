const int INA = 6; 
const int INB = 5; 
const int pushButton = 7;
const int ledPin = 13; // onboard LED
int action = 0; // 0 = Stop, 1 = Forward, 2 = Reverse

byte speed = 80; // change this to alter speed 0 - 255 although 80 is the minimum for reliable operation
 
void setup() 
{ 
  pinMode(INA,OUTPUT); 
  pinMode(INB,OUTPUT); 
  pinMode(pushButton, INPUT);
  pinMode(ledPin, OUTPUT); // onboard LED
  Serial.begin(9600); // for debug
}
 
void loop() 
{
  if (digitalRead(pushButton) == LOW) { // see if button has been pressed
    digitalWrite(ledPin, HIGH); // turn on LED
    action = action + 1; // increment action
    if(action > 2){
      action = 0; // keep action in range
    }
    if (action > 0){
      digitalWrite(ledPin, HIGH); // Turn OFF LED if motor stopped
      stopFan();
      delay(100); // short delay to make direction change easier for motor
    }
    else   digitalWrite(ledPin, LOW); // turn onboard LED OFF
    delay(800); // simple switch debounce
  }

  switch (action){
    case 0:
    Serial.println("Stop Fan:");
    stopFan();
    break;

    case 1:
    Serial.println("Fan Forwards:");    
    forward();
    break;

    case 2:
    Serial.println("Fan Reverse:");
    reverse();
    break;
  }
}

void reverse() {
  analogWrite(INA,0);
  analogWrite(INB,speed);   
}


void forward() {
  analogWrite(INA,speed);
  analogWrite(INB,0); 
  
}

void stopFan() {
  digitalWrite(INA,LOW);
  digitalWrite(INB,LOW); 
}