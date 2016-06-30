
const int CSwitchModePin = 12;
const int CSwitchAcceptPin = 8;
const int CLedPin1 =  2;
const int CLedPin2 =  4;
const int CLedPin3 =  7;

void setup() {
  pinMode(CLedPin1, OUTPUT);
  pinMode(CLedPin2, OUTPUT);
  pinMode(CLedPin3, OUTPUT);
  pinMode(CSwitchModePin, INPUT);
  pinMode(CSwitchAcceptPin, INPUT);

  Serial.begin(9600);
}

void loop() {
  digitalWrite(CLedPin1, HIGH);
  digitalWrite(CLedPin2, LOW);
  digitalWrite(CLedPin3, LOW);
  delay(500);
  digitalWrite(CLedPin1, LOW);
  digitalWrite(CLedPin2, HIGH);
  digitalWrite(CLedPin3, LOW);
  delay(500);
  digitalWrite(CLedPin1, LOW);
  digitalWrite(CLedPin2, LOW);
  digitalWrite(CLedPin3, HIGH);
  delay(500);
  Serial.println("");
  Serial.print("Switch Mode: "); Serial.print(digitalRead(CSwitchModePin)); Serial.println("");
  Serial.print("Switch Accept: "); Serial.print(digitalRead(CSwitchAcceptPin)); Serial.println("");

  
}
