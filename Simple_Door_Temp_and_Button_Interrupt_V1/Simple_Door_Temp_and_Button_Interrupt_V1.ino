
/***************************************************
  Adafruit ESP8266 WiFi Sensor Module

Open/Close door with temperature
and
Toggle button using Interrupt

 ****************************************************/

// Libraries
#include <ESP8266WiFi.h>
#include "DHT.h"

//Variables
#define THRESHOLD_OPEN  76 //Set door open temperature
#define THRESHOLD_CLOSE 75 //Set door close temperature

// Configure DHT sensors 
DHT dht1(4, DHT11); //blue sensor. On Amico board GPIO 4 is D2

//Pin location
const int doorPin = 5; // Amico board GPIO 5 is D1
const int buttonPin = 2;  //Amico board GPIO 2 is D4 **note: GPIO16 will not interrupt
//Variable
volatile int buttonState = 0;  // variable set as volatile to be read outside of loop 
volatile int reading;
volatile int previous = HIGH;

long wait = 0;
long debounce = 500; // time to keep from false button reading

/*************************** Sketch Code ************************************/

void setup() {

  // Initilize sensors
  dht1.begin();

  // Configure pins for desired type
  pinMode (doorPin, OUTPUT);
  pinMode (buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), pin_ISR, CHANGE); // Set GPIO2 to monitor as interrupt

  // set connection speed
  Serial.begin(115200); 
  delay(10);
  Serial.println(F("Serial Connection Established"));  // Not printing yet?
  
}//end-setup

/***********************************************************************/
void loop() {
  
  int doorTemp = (int)dht1.readTemperature(true); //True reads Fehrenheit temp 
  Serial.print(F("Current Temp: "));
  Serial.println(doorTemp);
  Serial.println();           // blank line

/////////////////////////////////////////////
//Set Door position open/closed based on temp
/////////////////////////////////////////////
    if(doorTemp >= THRESHOLD_OPEN){
      digitalWrite(doorPin, HIGH);//set pin to high
      Serial.println(F("Pin set High - close circuit"));
    } 
    else if(doorTemp <= THRESHOLD_CLOSE){
      digitalWrite(doorPin, LOW);//
      Serial.println(F("Pin set Low - open circuit"));
    } 
   //read actual pin setting high/low and print door position to screen 
    if (digitalRead(doorPin) == 1){  // pin is equal to HIGH
      Serial.println(F("Door is OPEN"));
    }
    else {                           // else pin is LOW
       Serial.println(F("Door is currently CLOSED"));
    }
    Serial.print(F("Actual Pin Reading = ")); // display actual pin reading
    Serial.println(digitalRead(doorPin));
    Serial.println();

  // Time between temp readings (example: 60,000 would be 1 minute)
  delay(10000);

}//end-Loop

/*************************************************************/
//Interupt State - monitor button
void pin_ISR(){
  // read the state of the pushbutton value:
  reading = digitalRead(buttonPin);

  // TOGGLE 
  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the wait
  if (reading == HIGH && previous == LOW && millis() - wait > debounce) {
    if (buttonState == HIGH)
      buttonState = LOW;
    else
      buttonState = HIGH;

    wait = millis();    
  }

  digitalWrite(doorPin, buttonState); //set doorPin to toggled position

  previous = reading;  // set previous to current reading
 }

