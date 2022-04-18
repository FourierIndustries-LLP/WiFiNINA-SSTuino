/*
  Ubidots - 5 - Demo Program
  This example shows how to connect to Ubidots IoT using
  a SSTuino II perform send and receive operations

  Written by Pan Ziyue for FourierIndustries
  Licensed under the Creative Commons 0 (CC0) License and
  released to public domain
  <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include <WiFiNINA.h>
#include <MQTT.h>
#include "arduino_secrets.h"
#include "helper.h"

WiFiClient network;
MQTTClient client;

/********* This is where you put your own code *********/

// Variables
int temp = 0;
int light = LOW;
int motion = LOW;

// Variables for multitasking
unsigned long previousMillis = 0;
const long interval = 5000; // 5 seconds interval

/**************** End of your own code *****************/

void setup()
{
  Serial.begin(9600);

  /********* This is where you put your own code *********/

  pinMode(6, INPUT);
  pinMode(A0, INPUT);
  pinMode(2, OUTPUT);

  /**************** End of your own code *****************/

  connectToWiFi(SECRET_SSID, SECRET_PASS); // Connect to the WiFi network

  // Setup the MQTT client, but not connecting yet
  client.begin(MQTT_URL, MQTT_PORT, network);
  client.onMessage(messageReceived);

  connect(client, &subscribe); // Connect to MQTT server
}

void loop()
{
  bool connected = client.loop(); // This function must be called on every loop

  // Check if MQTT connection is active, and reconnect if it's broken
  // This must be called every loop to ensure connection
  if (connected == false)
  {
    Serial.println("MQTT Client disconnected, reconnecting...");
    connect(client, &subscribe);
  }

  /********* This is where you put your own code *********/

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    // Read the temperature
    temp = -40 + 0.488155 * (analogRead(A0) - 20);

    // Convert the temperature to a String and publish it
    String temperatureString = String(temp);
    publishUbidots(client, "sstuino-ii/temperature", temperatureString);

    // Read the motion data
    motion = digitalRead(6);

    // Convert motion to a String and publish it
    String motionString = String(motion);
    publishUbidots(client, "sstuino-ii/motion", motionString);
  }

  // Output the light switch
  digitalWrite(2, light);

  /**************** End of your own code *****************/
}

/* This is where we process all incoming transmissions */
void messageReceived(String &topic, String &payload)
{
  Serial.println("Incoming data: " + topic + " - " + payload);

  /*
    All the messages your MQTT service receives can be
    loaded here

    *NOTE*: Do not use this function to publish, subscribe
    or unsubscribe as it may cause deadlocks
  */

  /********* This is where you put your own code *********/

  if (topicMatches("sstuino-ii/light", topic) == true)
  {
    light = payload.toInt(); // convert the incoming message from String to int
  }

  /**************** End of your own code *****************/
}

/* Subscribe to topics you need here! */
void subscribe()
{
  /*
    For example, a subscribe is called like:
    subscribeUbidots(client, "sstuino-ii/light");
  */

  /********* This is where you put your own code *********/

  subscribeUbidots(client, "sstuino-ii/light");

  /**************** End of your own code *****************/
}
