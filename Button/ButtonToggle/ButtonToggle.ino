/*
Use pushButton to toggle pin on/off

 */

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int doorPin =  5;      // the number of the solenoid

// variables will change:
int buttonState = LOW;         // variable for reading the pushbutton status
int reading;
int previous = HIGH;

long time = 0;
long debounce = 200;

void setup() {
  // initialize the ledLED pin as an output:
  pinMode(doorPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

}//End Setup

void loop() {
  // read the state of the pushbutton value:
  reading = digitalRead(buttonPin);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (reading == LOW && previous == HIGH && millis() - time > debounce) {
    if (buttonState == LOW)
      buttonState = HIGH;
    else
      buttonState = LOW;

    time = millis();    
  }

  digitalWrite(doorPin, buttonState); //set doorPin to toggled position

  previous = reading;

}//End Loop
