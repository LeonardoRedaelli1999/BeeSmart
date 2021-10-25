/*
 * PIR sensor tester
 */
 
int ledPin = 13;                // choose the pin for the LED
int pirPin = 7;               // pin for PIR sensor
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;
long count=0;
 
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
}

// speed of acquisition of datas is controlled via potentiometers on the PIR
void loop(){
  val = digitalRead(pirPin);  // read PIR value
  if (val == HIGH) {
    digitalWrite(ledPin, HIGH);  // if motion turn LED ON
    if (pirState == LOW) {
      Serial.println("Motion detected!");
      count=count+1;
      Serial.print("Number of activations:");
      Serial.println(count);
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW); // turn LED OFF
    if (pirState == HIGH){
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
}
