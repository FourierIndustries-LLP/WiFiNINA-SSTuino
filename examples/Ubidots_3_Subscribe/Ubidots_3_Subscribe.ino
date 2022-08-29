/*
  Ubidots - 3 - Subscribe
  This example shows how to connect to Ubidots IoT using
  a SSTuino II and receive data by Subscribing

  Written by Pan Ziyue for FourierIndustries
  Licensed under the Creative Commons 0 (CC0) License and
  released to public domain
  <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include <WiFiNINA.h>
#include <MQTT.h>
#include <RBD_Timer.h>
#include "arduino_secrets.h"
#include "helper.h"

WiFiClient network;
MQTTClient client;

// declaration

/********* This is where you put your own code *********/

// Variables
int light = LOW;

// Timer for multitasking. Default is 10000 ms (10 seconds)
RBD::Timer timer(10000);

/**************** End of your own code *****************/

void setup()
{
  Serial.begin(9600);

  /********* This is where you put your own code *********/

  pinMode(2, OUTPUT);

  /**************** End of your own code *****************/

  connectToWiFiAndMQTT(network, SECRET_SSID, SECRET_PASS, client, subscribe, messageReceived);
}

void loop()
{
  connectionCheck(client, subscribe); // Check if connection is ok, must run on every loop

  /*
    If you need to publish data here, a publish is called 
    with a timer like this:

    if (timer.onRestart())
    {
      String dataString = String(data);
      publishUbidots(client, "topicName", dataString);
    }

    The value passed to publishUbidots must be a String
  */

  /********* This is where you put your own code *********/

  digitalWrite(2, light);

  /**************** End of your own code *****************/
}

/* This is where we process all incoming transmissions */
void messageReceived(String &topic, String &payload)
{
  Serial.println("Incoming data: " + topic + " - " + payload);

  /*
    All the messages your MQTT service receives can be
    loaded here. You should check if the message is the
    correct topic and convert the message from String
    to the data type you need

    For example:
    if (topicMatches("light", topic) == true)
    {
      light = payload.toInt(); // convert incoming message from String to int
    }

    *NOTE*: Do not use this function to publish, subscribe
    or unsubscribe as it may cause deadlocks
  */

  /********* This is where you put your own code *********/

  if (topicMatches("light", topic) == true)
  {
    // You can do something with the payload, which is a String
    light = payload.toInt(); // convert the incoming message from String to int
  }

  /**************** End of your own code *****************/
}

/* Subscribe to topics you need here! */
void subscribe()
{
  /*
    For example, a subscribe is called like:
    subscribeUbidots(client, "light");
  */
 
  /********* This is where you put your own code *********/

  subscribeUbidots(client, "light");

  /**************** End of your own code *****************/
}
