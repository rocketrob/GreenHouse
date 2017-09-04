/***************************************************
  Adafruit ESP8266 WiFi Sensor Module

Open/Close door with temperature
Interrupt and open door with button
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
volatile int buttonState = 0;  //variable for reading pushbutton status
int reading;
int previous = HIGH;

long time = 0;
long debounce = 200;

void setup() {

  // Initilize sensors
  dht1.begin();

  // Configure pins for desired type
  pinMode (doorPin, OUTPUT);
  pinMode (buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), pin_ISR, CHANGE); // GPIO2 is Index vector 4, 
                                                              // Use digitalPinToInterrupt(pin) instead of vector
                                                              // set to monitor interupt
                                                              
  // set connection speed
  Serial.begin(115200); 
  delay(10);
  Serial.println(F("Serial Connection Established"));
  
}//end-setup

/***********************************************************************/
void loop() {
  
  int doorTemp = (int)dht1.readTemperature(true); //True reads Fehrenheit temp 
  Serial.print(F("Current Temp: "));
  Serial.println(digitalRead(doorTemp));
  Serial.println();

/////////////////////////////////////////////
//Set Door position open/closed based on temp
/////////////////////////////////////////////
    if(doorTemp >= THRESHOLD_OPEN){
      digitalWrite(doorPin, HIGH);//set pin to low
      Serial.println(F("Pin set High - close circuit"));
    } 
    else if(doorTemp <= THRESHOLD_CLOSE){
      digitalWrite(doorPin, LOW);//High equals powered
      Serial.println(F("Pin set Low - open circuit"));
    } 
   //read actual pin setting high/low and print door position to screen 
    if (digitalRead(doorPin) == 1){
      Serial.println(F("Door is OPEN"));
    }
    else {
       Serial.println(F("Door is currently CLOSED"));
    }
    Serial.print(F("Actual Pin Reading = "));
    Serial.println(digitalRead(doorPin));
    Serial.println();

  // To Repeat every (60,000 milSec) 1 Minutes Change delay to change refresh rate
  delay(5000);

}//end-Loop

/*************************************************************/
//Interupt State - monitor button
void pin_ISR(){
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
  
} 

