#ifndef SSTUINO_HELPERFILE
#define SSTUINO_HELPERFILE

#include <WiFiNINA.h>
#include <MQTT.h>

/* This function allows a fault-tolerant way to connect to
   WiFi, by shutting down the WiFi chip if a connection
   error occurs, and turns it back on to reattempt
   connection */
void connectToWiFi(const char *ssid, const char *password)
{
  Serial.println("Connecting to WiFi...");
  int wifiStatus = WiFi.begin(ssid, password);
  unsigned long delayTime = 2000;
  while (wifiStatus != WL_CONNECTED)
  {
    WiFi.end(); // shut down the WiFi radio and try again
    Serial.println("Retrying WiFi connection...");
    wifiStatus = WiFi.begin(ssid, password);
    delay(delayTime);
    delayTime += 2000; // add 2s to delay to prevent spamming the radio
    wifiStatus = WiFi.status();
    Serial.print("WiFi State: ");
    Serial.println(wifiStatus);
  }
  Serial.println("Connected to wifi");

  WiFi.setDNS(IPAddress(1, 1, 1, 1), IPAddress(1, 0, 0, 1)); // Set DNS to avoid router DNS issues
}

/* This function allows you to publish an MQTT message to
   Ubidots IoT painlessly */
bool publishUbidots(MQTTClient &client, const char *channel, String &message)
{
  char buffer_topic[64];
  char buffer_message[32];
  snprintf(buffer_topic, 64, "/v1.6/devices/%s", channel);
  snprintf(buffer_message, 32, "{\"value\": %s}", message.c_str());
  return client.publish(buffer_topic, buffer_message);
}

/* This function allows you to subscribe to MQTT messages
   from Ubidots IoT painlessly */
bool subscribeUbidots(MQTTClient &client, const char *channel)
{
  char buffer_topic[64];
  snprintf(buffer_topic, 64, "/v1.6/devices/%s/lv", channel);
  return client.subscribe(buffer_topic);
}

/* This function checks if your channel matches the
   subscribed topic */
bool topicMatches(const char *channel, String &topic)
{
  char buffer_topic[64];
  snprintf(buffer_topic, 64, "/v1.6/devices/%s/lv", channel);
  return strncmp(buffer_topic, topic.c_str(), 64) == 0 ? true : false;
}

/* This function handles WiFi and MQTT connection
   (and reconnection when the connection fails), as well as
   subscribing to new topics */
void connect(MQTTClient &client, void (*subscribe)(void))
{
  // Reconnect to WiFi, if the connection is broken
  if (WiFi.status() != WL_CONNECTED)
  {
    connectToWiFi(SECRET_SSID, SECRET_PASS);
  }

  Serial.print("Connecting to MQTT");
  while (!client.connect(MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD))
  {
    int lastError = client.lastError();
    // an error code of -9 is okay, it just means it will take some time
    if (lastError == -9)
    {
      Serial.print(".");
    }
    else if (lastError == -3)
    {
      // network error, did Wi-Fi disconnect itself?
      Serial.print("\nNetwork error encountered, current WiFi status is: ");
      int wifiStatus = WiFi.status();
      Serial.println(wifiStatus);
      // Force a reconnect if WiFi disconnected by itself
      if (wifiStatus == WL_DISCONNECTED || wifiStatus == WL_CONNECTION_LOST)
      {
        connectToWiFi(SECRET_SSID, SECRET_PASS);
      }
    }
    else
    {
      Serial.print("\nMQTT Error: ");
      Serial.println(client.lastError()); // other error codes will be printed out
    }
    delay(2500);
  }

  Serial.println("\nConnected to MQTT!");
  subscribe();
}

/*
  ********** Quick Status/Error Codes Reference **********

  This section allows you to quickly look up all the errors
  and status codes without having to browse the official
  documentation

  WiFi Status Codes:

  WL_IDLE_STATUS      0: Doing nothing
  WL_NO_SSID_AVAIL    1: No WiFi networks available
  WL_SCAN_COMPLETED   2: Finished WiFi scan
  WL_CONNECTED        3: Connected to network
  WL_CONNECT_FAILED   4: Failed to connect
  WL_CONNECTION_LOST  5: Lost connection
  WL_DISCONNECTED     6: Currently not connected
  WL_NO_MODULE        255: Something's wrong with the WiFi chip

  MQTT Error Codes:

  LWMQTT_SUCCESS                     0: Successfully connected
  LWMQTT_BUFFER_TOO_SHORT           -1: We ran out of RAM! You may have to increase the buffer size.
  LWMQTT_VARNUM_OVERFLOW            -2: We ran out of RAM!
  LWMQTT_NETWORK_FAILED_CONNECT     -3: Can't connect to network
  LWMQTT_NETWORK_TIMEOUT            -4: Network not responding
  LWMQTT_NETWORK_FAILED_READ        -5: Can't receive anything
  LWMQTT_NETWORK_FAILED_WRITE       -6: Can't send anything
  LWMQTT_REMAINING_LENGTH_OVERFLOW  -7: We ran out of RAM!
  LWMQTT_REMAINING_LENGTH_MISMATCH  -8: Data we received is inconsistent
  LWMQTT_MISSING_OR_WRONG_PACKET    -9: Sometimes this happens due to minor network issues
  LWMQTT_CONNECTION_DENIED          -10: Server denied connection
  LWMQTT_FAILED_SUBSCRIPTION        -11: Subscribe failed
  LWMQTT_SUBACK_ARRAY_OVERFLOW      -12: Data we received is inconsistent
  LWMQTT_PONG_TIMEOUT               -13: Heartbeat signal sent to server did not receive response, network failed?
*/

#endif