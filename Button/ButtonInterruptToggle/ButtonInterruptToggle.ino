/*
  Toggle Button with Interrupt

 Use interupt to monitor pushButton to toggle doorPin on/off
 Configuration:
 Button pin is wired to have constant voltage until button pressed grounds it out.

 DoorPin used to trigger relay switch & drive 12V Linear slide motor

 */

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int doorPin =  5;      // the number of the solenoid

// variables
volatile int buttonState = 0; // set as volatile because it's used as interupt
int reading;
int previous = HIGH;

long wait = 0;
long debounce = 200;

void setup() {
  // initialize the doorPin as an output:
  pinMode(doorPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(2), pin_ISR, CHANGE);
}//End Setup

void loop() {
  // Empty loop will monitor temperature and set doorPin
}//End Loop

void pin_ISR(){
  // read the state of the pushbutton value:
  reading = digitalRead(buttonPin);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the wait
  if (reading == LOW && previous == HIGH && millis() - wait > debounce) {
    if (buttonState == LOW)
      buttonState = HIGH;
    else
      buttonState = LOW;

    wait = millis();    
  }

  digitalWrite(doorPin, buttonState); //set doorPin to toggled position

  previous = reading;
  
}  

