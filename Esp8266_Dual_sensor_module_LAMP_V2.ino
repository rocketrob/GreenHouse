/***************************************************
  Adafruit ESP8266 WiFi Sensor Module

  ATTEMPT TO TRIGGER DOOR BY TEMP
  
  Set up for Two (2) sensor feeds and Lamp control to AIO Dashboard
  Using the Huzzah ESP8266 breakout board
    Note: to upload sketch you must place ESP8266 into Boot Mode by
      Hold down the GPIO0 button, the red LED will be lit
      While holding down GPIO0, click the RESET button
      Release RESET, then release GPIO0
      When you release the RESET button, the red LED will be lit dimly, this means its ready to bootload
  
  This program is written for two DHT11 (low range) or DHT22 (high range) sensors
  and two feeds and one powerSwitch tail kit

  // additional sensors can be added or commented out as needed
 ****************************************************/

// Libraries
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

// WiFi parameters
#define WLAN_SSID       "Strawhouse"
//#define WLAN_PASS       "WLAN_PASS"  ** no password required

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "rocketrob"
#define AIO_KEY         "d4b0aa4ae08643938438a77f5a9015f9"

//Variables
#define THRESHOLD_OPEN  70 //Set door open temperature
#define THRESHOLD_CLOSE 60 //Set door close temperature

// Configure DHT sensors 
    ///////////////////////////////////////////////////////////////
    //Parameters are (PIN, SENSORTYPE, 15) "don't know what the 15 is for"
    // Sensor types:  
    // DHT 11  This is the 'blue' lower range sensor
    // DHT 22  This is the 'white' higher range sensor
    ///////////////////////////////////////////////////////////////
// Configured DHT sensors instance.  Add or comment out additional sensors here
DHT dht1(4, DHT11, 15); //blue sensor
//DHT dht2(2, DHT22, 15); //white sensor

//Configure PowerSwitch tail to an output pin
const int door_pin = 5;

// Functions
void connect();

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************Setup feeds for temperature & humidity for sensors******/
/////////////////////////////////////////////////////////////////////////////
// The first part "temp1" is the publish name used in the loop below.      //
// the next "/feeds/yourZoneName" is whatever you've created in IO.Adafruit//
/////////////////////////////////////////////////////////////////////////////
Adafruit_MQTT_Publish temp1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/doorTemp");
Adafruit_MQTT_Publish humidity1 = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/doorHum");
//Adafruit_MQTT_Publish temp2 = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/tempZone2");
//Adafruit_MQTT_Publish humidity2 = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/humidityZone2");

// Subscribe to your feed called 'door'
Adafruit_MQTT_Subscribe door = Adafruit_MQTT_Subscribe (&mqtt, AIO_USERNAME "/feeds/door");

/*************************** Sketch Code ************************************/

void setup() {

  // Initilize sensors
  dht1.begin();
  //dht2.begin();

  // Set door pin to output
  pinMode (door_pin, OUTPUT);
  
  Serial.begin(115200); //upload speed
  Serial.println(F("Adafruit IO Example"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID); //, WLAN_PASS) removed from parameter as Strawhouse does not have password
  while (WiFi.status() != WL_CONNECTED) { //while not connected print a dot every 1/2 sec ...
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  //listen to door feed
  mqtt.subscribe(&door);

  // connect to adafruit io
  connect();
  
}//setup

/***********************************************************************/
// Once connected, run this loop to start publishing sensor data.
void loop() {

  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }

  Adafruit_MQTT_Subscribe *subscription;

  // Grab the current state of the sensors as a data value to use in the publish data below
  int doorHum = (int)dht1.readHumidity();
  int doorTemp = (int)dht1.readTemperature(true); //True reads Fehrenheit temp
    //uncomment below if second sensor added
 // int humidity_data2 = (int)dht2.readHumidity();
 // int temperature_data2 = (int)dht2.readTemperature(true); //True reads Fehrenheit temp

  // Publish data Sensor1
  if (! temp1.publish(doorTemp)) //if not published
    Serial.println(F("Failed to publish doorTemp"));
  else
    Serial.println(F("doorTemp published!")); //else published
    Serial.println(doorTemp);

  if (! humidity1.publish(doorHum))
    Serial.println(F("Failed to publish humidityZone1"));
  else
    Serial.println(F("doorHum published!"));
    Serial.println(doorHum);
    
 /* 
  //uncomment below if second sensor added
  // Publish data Sensor2
 if (! temp2.publish(temperature_data2))
    Serial.println(F("Failed to publish temperature2"));
  else
    Serial.println(F("Temperature2 published!"));
    Serial.println(temperature_data2);

  if (! humidity2.publish(humidity_data2))
    Serial.println(F("Failed to publish humidity2"));
  else
    Serial.println(F("Humidity2 published!"));
    Serial.println(humidity_data2);
*/

  // To Repeat every (60,000 milSec) 1 Minutes Change delay to change refresh rate
  delay(5000);

//////////////////////////////////////////
//open/close based on Adafruit.io feed  
///////////////////////////////////////// 
/*
//Read Lamp setting
  while(subscription = mqtt.readSubscription(1000)) {
    //only check lamp events
    if(subscription == &door){
      //convert mqtt ascii to int
      char *value = (char *)door.lastread;
      Serial.print(F("Received: "));
      Serial.println(value);

      //message to door
      String message = String(value);
      message.trim();
      if(message == "ON") {digitalWrite(door_pin, HIGH);} //High equals powered
      if(message == "OFF") {digitalWrite(door_pin, LOW);} //Low equals no power
      }
    }//while
*/
/////////////////////////////////////////////
//Set Door position open/closed based on temp
/////////////////////////////////////////////
    if(doorTemp >= THRESHOLD_OPEN){
      digitalWrite(door_pin, HIGH);//High equals powered
      Serial.println(F("Door Open"));
    } 
    if(doorTemp <= THRESHOLD_CLOSE){
      digitalWrite(door_pin, LOW);//High equals powered
      Serial.println(F("Door Closed"));
    } 

}//Loop

/*************************************************************/
// Sub-Method used above to connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Connecting to Adafruit IO... "));

  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {

    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);

  }

  Serial.println(F("Adafruit IO Connected!"));

}
