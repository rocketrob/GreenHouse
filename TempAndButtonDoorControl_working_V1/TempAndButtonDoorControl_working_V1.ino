/***************************************************
  Adafruit ESP8266 WiFi Sensor Module **need to add this functionality back

  All within MAIN LOOP:
  Continually monitor if over-ride button is pressed. Open if pressed & hold for 10 seconds.
  If NOT pressed
  Set door position based on temperature
  then return monitoring button
 ****************************************************/

// Libraries
#include <ESP8266WiFi.h>
#include "DHT.h"

//Variables
#define THRESHOLD_OPEN  70 //Set door open temperature
#define THRESHOLD_CLOSE 65 //Set door close temperature

// Configure DHT sensors
DHT dht(4, DHT11); //blue sensor. On Amico board GPIO 4 is D2

//Pin location
const int doorPin = 5; // Amico board GPIO 5 is D1
const int buttonPin = 2;  //Amico board GPIO 2 is D4 **note: GPIO16 will not interrupt

//Variable
int buttonState = 0;  //variable for reading pushbutton status


/*************************** Sketch Code ************************************/

void setup() {

  // Initilize sensors
  dht.begin();

  // Configure pins for desired type
  pinMode (doorPin, OUTPUT);
  pinMode (buttonPin, INPUT);

  // set connection speed
  Serial.begin(115200);
  delay(100);
  Serial.println(F("Serial Connection Established"));

}//end-setup

/***********************************************************************/
void loop() {

          //Read thermometer value
        float doorTemp = (int)dht.readTemperature(true); //True reads Fehrenheit temp
        Serial.print(F("Current Temp: "));
        Serial.println(doorTemp);
        Serial.println();
  /////////////////////////////////////////////
  //Check for doorButton to be pressed
  /////////////////////////////////////////////

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin); // Button Input HIGH is released, LOW is pressed

      // if the button is pressed - trigger door open and wait for 10 seconds
  if (buttonState == LOW )  {

    digitalWrite(doorPin, HIGH); //set doorPin to toggled position above

    Serial.print(F("Holding door OPEN"));
    Serial.println();

    delay(5000); //hold door position for xxMilli seconds before returning to loop and reading temp.

    //digitalWrite(doorPin, LOW); //reset door pin to Low ** DON'T BELIEVE THIS IS NEEDED

  }//end checking button
   
    //else if button not pressed
    //set door position to match temperature reading
  
  else if (doorTemp >= THRESHOLD_OPEN) {
    digitalWrite(doorPin, HIGH);//set pin to HIGH (which is powered)
    Serial.println(F("Pin set High - close circuit"));
  }
  else if (doorTemp <= THRESHOLD_CLOSE) {
    digitalWrite(doorPin, LOW);// set pin to LOW
    Serial.println(F("Pin set Low - open circuit"));
  }

    //Finally read actual pin setting high/low and print door position to screen
  if (digitalRead(doorPin) == 1) {
    Serial.println(F("Door is OPEN"));
  }
  else {
    Serial.println(F("Door is currently CLOSED"));
  }
  
  delay(500); //pause between reading

}//end-Loop
